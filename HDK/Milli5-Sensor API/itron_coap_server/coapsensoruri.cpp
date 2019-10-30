/*

Copyright (c) Silver Spring Networks, Inc. 
All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the ""Software""), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of Silver Spring Networks, Inc. 
shall not be used in advertising or otherwise to promote the sale, use or other 
dealings in this Software without prior written authorization from Silver Spring
Networks, Inc.

*/


#include "log.h"
#include "hbuf.h"
#include "coapextif.h"
#include "coappdu.h"
#include "coapsensoruri.h"
#include "coapobserve.h"
#include "coapsensorobs.h"
#include "arduino_time.h"


/*! @brief
 * Access is serialized via events, only one client too. Only need to handle
 * one observe per sensor. We have 3 sensors.
 * To keep memory usage down, we only support 1 callback function. This can be changed.
 */
#define NUM_ARDUINO_RESOURCES   (crdt_max_sens - crdt_min_sens + 1)
static void *coap_hdl[NUM_ARDUINO_RESOURCES] = { NULL };
static uint8_t hdr_rsv_bytes;
struct coap_stats coap_stats;


// System handler forwards
static error_t crtitle(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp);

static error_t crwellknown(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp);

static error_t crsystem(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp);

// Dispatcher implemented in the "Sketch"
error_t crarduino( struct coap_msg_ctx *req, struct coap_msg_ctx *rsp );

// Dispatcher implemented in the Sensor API
error_t crsapi( struct coap_msg_ctx *req, struct coap_msg_ctx *rsp );

// Sensor API flag. Is 1 id using SAPI.
extern uint8_t is_sapi;


/* CoRE Link Attributes - RFC 6690 
 * Resource Type 'rt' Attribute - 
   The Resource Type 'rt' attribute is an opaque string used to assign
   an application-specific semantic type to a resource.  One can think
   of this as a noun describing the resource. 
 * Interface Description 'if' Attribute
   The Interface Description 'if' attribute is an opaque string used to
   provide a name or URI indicating a specific interface definition used
   to interact with the target resource.  One can think of this as
   describing verbs usable on a resource.  The Interface Description
   attribute is meant to describe the generic REST interface to interact
   with a resource or a set of resources.  
 */


#define S_URI_SYSTEM			"sys"
#define S_TIME_URI				"time"
#define S_STAT_URI              "stats"

#define S_STAT_URI_Q_MODULE     "mod"
#define S_STAT_URI_Q_MOD_COAP   S_STAT_URI_Q_MODULE "=coap"
#define S_STAT_URI_Q_MOD_PWR    S_STAT_URI_Q_MODULE "=pwr"
#define S_STAT_URI_Q_MOD_HDLC   S_STAT_URI_Q_MODULE "=hdlc"

#define CLA_SYSTEM  "if=" "\"" S_URI_SYSTEM "\"" ";title=\"System\";ct=42;rev=1;"
#define CLA_ARDUINO   "if=" "\"" DEFAULT_CLASSIFIER "\"" ";title=\"Arduino Sensors\";ct=42;"


// Table for the CoAP handler registry. Specific to this CoAP Server and Device.
// Note: We need only one sensor call back slot as we use a two level dispatching scheme.
//       First level is a <classifier> (e.g. "arduino")
//       Second level is a sensor resource (e.g. "temp")
//       If you add handlers for additional classifiers than you will need to add space table slots.
#define COAP_N_BASE_CBR		(2)
#define COAP_N_SYS_CBR		(1)
#define COAP_N_SENSOR_CBR	(1)

#define COAP_MAX_CBR        (COAP_N_BASE_CBR + COAP_N_SYS_CBR + COAP_N_SENSOR_CBR)


struct coap_cb_reg coap_registry[COAP_MAX_CBR];
static int		coap_reg_size = 0;


// URL Classifier
char			classifier[CLASSIFIER_MAX_LEN] = DEFAULT_CLASSIFIER;

// RTC Clock
extern RTCZero	rtc;


error_t coap_uri_register(const char *path, coap_cb cbfunc, const char *corelink)
{
    int idx;

    if (coap_reg_size < COAP_MAX_CBR)
	{
        idx = coap_reg_size++;    
    }
	else
	{
        return ERR_NO_MEM;
    }
    
    coap_registry[idx].path = path;
    coap_registry[idx].cb = cbfunc;
    coap_registry[idx].link = corelink;
    
    return ERR_OK;
}


// Initialize the CoAP registry
void coap_registry_init()
{
	/* Clear the registry */
	coap_reg_size = 0;

    /* mandatory elements */
    (void)coap_uri_register("", crtitle, "title=\"Info\";ct=0");
    (void)coap_uri_register(".well-known", crwellknown, NULL);

    /* basic resources from the NIC */
    (void)coap_uri_register(S_URI_SYSTEM, crsystem, CLA_SYSTEM);
	
	/*
	* Since coap_s_uri_proc only matches on the first path option, and it's
	* against the whole path, registering L_URI_LOGISTICS guarantees a match
	* or that URI, and its children. Putting it first means it's found faster.
	* We will never match the subsequent L_URI_LOGISTICS paths due to the
	* separation of the options but the concatenation of the path.
	* TODO Optimize this so that only one URI is required to be registered
	* while satisfying .well-known/core discovery.
	*/
	if (is_sapi == 1)
	{
		(void)coap_uri_register(classifier, crsapi, CLA_ARDUINO);
	}
	else
	{
		(void)coap_uri_register(classifier, crarduino, CLA_ARDUINO);
	}
}


// CoAP Dispatcher. Dispatch request to registered handler
error_t coap_s_uri_proc(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp)
{
    int i;
    int rs = sizeof(coap_registry) / sizeof (coap_registry[0]);
    error_t rc;
    struct optlv *op;

    if ((op = copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, NULL)) == NULL) 
	{
        i = 0;
    } 
	else 
	{
        for (i = 0; i < rs; i++) 
		{
            if (!coap_opt_strcmp(op, coap_registry[i].path)) 
			{
                break;
            }
        }
    }
    
    if (i == rs) 
	{
        /* no match found */
        rsp->code = COAP_RSP_404_NOT_FOUND;
        goto done;
    }

    rc = coap_registry[i].cb(req, rsp);

    if (rc != ERR_OK) 
	{
        rsp->code = COAP_RSP_500_INTERNAL_ERROR;
    }
done:
    /*
     * Some sort of error code, so can't observe.
     */
    op = copt_get_next_opt_type((const sl_co*)&(rsp->oh), COAP_OPTION_OBSERVE, NULL);
    if ((COAP_CLASS(rsp->code) != 2) || (op == NULL)) 
	{
        (void)copt_del_opt_type((sl_co*)&(rsp->oh), COAP_OPTION_OBSERVE);
        rsp->final = 1;
    }

    if (req->type == COAP_T_CONF_VAL) 
	{
        rsp->type = COAP_T_ACK_VAL;
    } 
	else 
	{
        rsp->type = COAP_T_NCONF_VAL;
    }

    return ERR_OK;
}


/*** Basic handlers ***/


// crtitle. Handles "/{prefix}". For example "/snsr".
static error_t crtitle(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp)
{
    if (req->code == COAP_REQUEST_GET)
	{
		char *d;
		char id[64];
		
		strcpy(id, COAP_SERVER_VERSION_STRING);
		strcat(id, COAP_SERVER_VERSION_NUMBER);
		
        d = (char*) m_append(rsp->msg, strlen(id));
        if (!d)
		{
            coap_stats.no_mbufs++;
            rsp->code = COAP_RSP_500_INTERNAL_ERROR;
        }
        else
		{
            memcpy(d, id, strlen(id));
            rsp->plen = strlen(id);
            rsp->cf = COAP_CF_TEXT_PLAIN;
            rsp->code = COAP_RSP_205_CONTENT;
        }            
    }
    else
	{
        rsp->code = COAP_RSP_403_FORBIDDEN;
    }
    return ERR_OK;
}


static error_t crwellknown(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp)
{
    int i;
    int rs = coap_reg_size; //sizeof(coap_registry) / sizeof (coap_registry[0]);
    char *ls;
    struct optlv *op;
    int n, m, len;
    struct coap_cb_reg *cr;
    void *it = NULL;

    rsp->code = 0;  /* unknown yet - fill in below */
    if (req->code == COAP_REQUEST_GET) {
        /* confirm we have two Uri-Path elements, not less, not more */
        /* the first one is .well-known, second one must be 'core'   */
        if (!copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it)    || 
            !(op = 
                copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it)) ||
            coap_opt_strcmp(op, "core")                                      ||
            copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it)) {
        
            rsp->code = COAP_RSP_404_NOT_FOUND;
            return ERR_FAIL;
        }

        for (i = 2; i < rs; i++) {  /* skipping default and well-known */
            cr = coap_registry + i;
            len = strlen(cr->path) + 4;         /* adding </ > , */
            if (cr->link) {
                len += strlen(cr->link) + 1;    /* ; */
            }
            
            ls = (char*) m_append(rsp->msg, len);
            if (!ls) {
                coap_stats.no_mbufs++;
                rsp->code = COAP_RSP_500_INTERNAL_ERROR;
                return ERR_FAIL;
            }

            n = 2;
            memcpy(ls, "</", n);
            m = strlen(cr->path);
            memcpy(&(ls[n]), cr->path, m);
            n += m;
            ls[n++] = '>';
            if (cr->link) {
                ls[n++] = ';';
                memcpy(&(ls[n]), cr->link, strlen(cr->link));
            }
            ls[len-1] = ',';
            /* no NUL terminator here */
            rsp->code = COAP_RSP_205_CONTENT;
        }

        rsp->cf = COAP_CF_APPLICATION_LINK_FORMAT; /* application/link-format */
        rsp->plen = rsp->msg->m_pktlen;
    }
    else {
        rsp->code = COAP_RSP_501_NOT_IMPLEMENTED;
    }

    return ERR_OK;
}


 // crsystem_time. Handles "/{prefix}/sys/time". For example "/snsr/sys/time".
static error_t crsystem_time(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it)
{
    struct optlv *o;

    /* No URI path beyond /time is supported, so reject if present. */
    if ((o = copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it)))
	{
        rsp->code = COAP_RSP_404_NOT_FOUND;
        rsp->plen = 0;
        return ERR_OK;
    }            
   
    if (req->code == COAP_REQUEST_GET)
	{
		uint32_t now;
        void *d;
        int len;
		
		now = get_rtc_epoch();
		dlog(LOG_DEBUG, "Epoch for GET of sys time: %08x", now);
        now = htonl(now);
        len = sizeof(now);
        d = m_append(rsp->msg, len);
        if (!d)
		{
            coap_stats.no_mbufs++;
            return ERR_NO_MEM;
        }
        memcpy(d, &now, len);
        rsp->plen = len;
        rsp->cf = COAP_CF_APPLICATION_OCTET_STREAM;
        rsp->code = COAP_RSP_205_CONTENT;
    }
	else if (req->code == COAP_REQUEST_PUT)
	{
        m_adj(req->msg, req->hdrlen);
        coap_sys_time_data_t *td = mtod(req->msg, coap_sys_time_data_t *);
        //ddump(LOG_DEBUG, "PUT /sys/time Payload", (void *)td, sizeof(coap_sys_time_data_t));

        /* Ensure type and length correct */
        if ((req->msg->m_pktlen == 0) || ((td->tl.u.rdt != crdt_time_abs) && (td->tl.u.rdt != crdt_time_delta)) || 
            (td->tl.l != (sizeof(coap_sys_time_data_t) - sizeof(coap_sens_tl_t))))
		{
            rsp->code = COAP_RSP_406_NOT_ACCEPTABLE;
        }
		else if (td->tl.u.rdt == crdt_time_abs)
		{
			uint32_t epoch;
			
			// Expect UTC time
			ddump(LOG_DEBUG, "PUT /sys/time Payload", (void *)td, sizeof(coap_sys_time_data_t));
			//#ifdef _ATMEL_BIGENDIAN_
			epoch = (uint32_t) ntohl(td->sec);
			//#else
			//epoch = bswap32(td->sec);
			//#endif 
			dlog(LOG_DEBUG, "Setting RTC to epoch: %08x", epoch);
			
			rtc.setEpoch(epoch);
			
			print_current_date();
			print_current_time();
			
			rsp->code = COAP_RSP_204_CHANGED;
			rsp->plen = 0;
        }
    }
	else
	{
        rsp->code = COAP_RSP_501_NOT_IMPLEMENTED;
        rsp->plen = 0;
    }
    
    return ERR_OK;
}

/*
 * Get the coap_stats data, with TLV.
 */
static error_t coap_get_coap_stats(struct mbuf *m, uint8_t *len)
{
    coap_sys_coap_stats_t *d = (coap_sys_coap_stats_t *) m_append(m, sizeof(coap_sys_coap_stats_t));
    if (!d) {
        coap_stats.no_mbufs++;
        return ERR_NO_MEM;
    }
    d->tl.u.rdt = crdt_stat_coap;
    d->tl.l = sizeof(coap_stats);
    d->cs.no_mbufs = htonl(coap_stats.no_mbufs);
    d->cs.no_mem = htonl(coap_stats.no_mem);
    d->cs.sensors_enabled = htonl(coap_stats.sensors_enabled);
    d->cs.sensors_disabled = htonl(coap_stats.sensors_disabled);
    d->cs.err_hdlc_send = htonl(coap_stats.err_hdlc_send);
    d->cs.active_obs = htonl(coap_stats.active_obs);
    d->cs.rx_success = htonl(coap_stats.rx_success);
    d->cs.tx_success = htonl(coap_stats.tx_success);
    d->cs.nretries_exceeded = htonl(coap_stats.nretries_exceeded);
    *len = sizeof(*d);

    return ERR_OK;
}


/*
 * Return or set, the specified system stats.
 */
static error_t crsystem_stats(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it)
{
    struct optlv *o;
    uint8_t len;
    error_t rc;

    /* No URI path beyond /stats is supported, so reject if present. */
    if ((o = copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it))) {
        rsp->code = COAP_RSP_404_NOT_FOUND;
        goto err;
    }            
    o = copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_QUERY, NULL);

    if (req->code == COAP_REQUEST_GET) {
        if (!coap_opt_strcmp(o, S_STAT_URI_Q_MOD_COAP)) {
            /* get CoAP stats */
            rc = coap_get_coap_stats(rsp->msg, &len);
        } else if (!coap_opt_strcmp(o, S_STAT_URI_Q_MOD_PWR)) {
            /* get power stats */
            // TODO: Do we need this?
            //rc = coap_get_pwr_stats(rsp->msg, &len);
            rc = ERR_INVAL;
        } else {
            /* Don't support other queries. */
            rsp->code = COAP_RSP_501_NOT_IMPLEMENTED;
            goto err;
        }
        dlog(LOG_DEBUG, "GET (status %d) read %d bytes.", rc, len);
        if (!rc) {
            rsp->plen = len;
            rsp->cf = COAP_CF_APPLICATION_OCTET_STREAM;
            rsp->code = COAP_RSP_205_CONTENT;
        } else {
            switch (rc) {
            case ERR_BAD_DATA:
            case ERR_INVAL:
                rsp->code = COAP_RSP_406_NOT_ACCEPTABLE;
                break;
            default:
                rsp->code = COAP_RSP_500_INTERNAL_ERROR;
                break;
            }
            goto err;
        }
    } else if (req->code == COAP_REQUEST_PUT) {
        if (!coap_opt_strcmp(o, S_STAT_URI_Q_MOD_COAP)) {
            /* Set CoAP stats */
            rsp->code = COAP_RSP_501_NOT_IMPLEMENTED;
            goto err;
        } else if (!coap_opt_strcmp(o, S_STAT_URI_Q_MOD_PWR)) {
            /* Set power stats */
            // TODO: Do we need this?
            //rc = coap_set_pwr_stats();
            rc = ERR_INVAL;
        } else {
            /* Don't support other queries. */
            rsp->code = COAP_RSP_501_NOT_IMPLEMENTED;
            goto err;
        }
        dlog(LOG_DEBUG, "SET (status %d).", rc);
        if (!rc) {
            rsp->plen = 0;
            rsp->code = COAP_RSP_204_CHANGED;
        } else {
            switch (rc) {
            case ERR_BAD_DATA:
            case ERR_INVAL:
                rsp->code = COAP_RSP_406_NOT_ACCEPTABLE;
                break;
            default:
                rsp->code = COAP_RSP_500_INTERNAL_ERROR;
                break;
            }
            goto err;
        }
    } else {
        rsp->code = COAP_RSP_501_NOT_IMPLEMENTED;
        goto err;
    }
    
    return ERR_OK;
err:
    rsp->plen = 0;

    return ERR_OK;
}


// The "sys" dispatcher.
static error_t crsystem(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp)
{
    struct optlv *o;
    void *it = NULL;
	
    /* No observes on system */
    if ((o = copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_OBSERVE, NULL)))
	{
        copt_del_opt_type((sl_co*)&(rsp->oh), COAP_OPTION_OBSERVE);
    }
    /* 
     * No URI path beyond /system, except /time and /stats is supported, so
     * reject if present. 
     */
    copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it);
	
    if ((o = copt_get_next_opt_type((const sl_co*) &(req->oh), COAP_OPTION_URI_PATH, &it)))
	{
        if (!coap_opt_strcmp(o, S_TIME_URI))
		{
            return crsystem_time(req, rsp, it);
        }
		else if (!coap_opt_strcmp(o, S_STAT_URI))
		{
            /* placeholder for stats */
            return crsystem_stats(req, rsp, it);
        }
		else
		{
            rsp->code = COAP_RSP_404_NOT_FOUND;
        }
    }
	else
	{
        rsp->code = COAP_RSP_404_NOT_FOUND;
    }

    rsp->plen = 0;

    return ERR_OK;
}
