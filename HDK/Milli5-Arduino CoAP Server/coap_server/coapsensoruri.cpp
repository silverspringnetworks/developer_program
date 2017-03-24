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


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "log.h"
#include "hbuf.h"
#include "coapextif.h"
#include "coappdu.h"
#include "coapsensoruri.h"
#include "coapobserve.h"
#include "coapsensorobs.h"
#include "arduino_time.h"

static error_t crtitle(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp);
static error_t crwellknown(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp);
static error_t crsystem(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp);

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

#define CLA_TITLE   "title=\"Info\";ct=0"

#define S_URI_SYSTEM        "system"

#define S_STAT_URI              "stats"
#define S_STAT_URI_Q_MODULE     "mod"
#define S_STAT_URI_Q_MOD_COAP   S_STAT_URI_Q_MODULE "=coap"
#define S_STAT_URI_Q_MOD_PWR    S_STAT_URI_Q_MODULE "=pwr"
#define S_STAT_URI_Q_MOD_HDLC   S_STAT_URI_Q_MODULE "=hdlc"

#define S_TIME_URI          "time"
#define S_STATS_URI         "stats"
#define S_TIME_URI_Q_ABS    "abs"
#define S_TIME_URI_Q_DELTA  "delta"
#define S_SV_URI            "sysvar"
#define S_SV_URI_Q_ID       "id="

#define S_UPG_URI   			"upg"
#define S_UPG_URI_Q_INFO        "info"
#define S_UPG_URI_Q_INFO_VER    S_UPG_URI_Q_INFO "=ver"
#define S_UPG_URI_Q_INFO_STATE    S_UPG_URI_Q_INFO "=sts"
#define S_UPG_URI_Q_STATE       "st"
#define S_UPG_URI_Q_STATE_INIT     S_UPG_URI_Q_STATE "=init"
#define S_UPG_URI_Q_STATE_VERIFY   S_UPG_URI_Q_STATE "=verify"
#define S_UPG_URI_Q_STATE_ACTIVATE S_UPG_URI_Q_STATE "=activate"
#define S_UPG_URI_Q_IMGOFFSET   "off="

#define CLA_SYSTEM  "if=" "\"" S_URI_SYSTEM "\"" ";title=\"System\";ct=42;rev=1;"

#define L_URI_LOGISTICS     "logistics"
#define L_URI_Q_CFG         "cfg"

#define L_SENS_URI              "sens"
#define L_SENS_URI_Q_ID         "id"
#define L_SENS_URI_Q_CFG_TILT   L_URI_Q_CFG "=tilt"
#define L_SENS_URI_Q_CFG_SHOCK  L_URI_Q_CFG "=shock"
#define L_SENS_URI_Q_CFG_TEMP   L_URI_Q_CFG "=temp"
#define L_SENS_URI_Q_CFG_HUMID  L_URI_Q_CFG "=humid"
#define L_SENS_URI_Q_ID_ALL     L_SENS_URI_Q_ID "=all"
#define L_SENS_URI_Q_ID_TILT    L_SENS_URI_Q_ID "=tilt"
#define L_SENS_URI_Q_ID_SHOCK   L_SENS_URI_Q_ID "=shock"
#define L_SENS_URI_Q_ID_TEMP    L_SENS_URI_Q_ID "=temp"

#define L_LOG_URI               "log"
#define L_LOG_URI_Q_RW          "rw="
#define L_LOG_URI_Q_SN          "sn="
#define L_LOG_URI_Q_LOGN        "logn="
#define L_LOG_URI_Q_NON         "non="
#define L_LOG_URI_Q_CFG_GLBL    L_URI_Q_CFG "=glbl"
#define L_LOG_URI_Q_NON         "non="

/* 
 * TODO CLA_LOGISTICS: Check ct, and see if format above can/should be
 * optimised. See https://tools.ietf.org/html/rfc6690.
 * Use #defines L_URI*
 */

#define L_URI_MBUS_WATER "mbus_water"
#define CLA_MBUS_WATER "if=" "\"" L_URI_MBUS_WATER "\"" ";title=\"Mbus Water Meter\";ct=42;"

#define L_URI_Q_TDATA       "tdata"
#define L_URI_Q_TDATA_ALL   L_URI_Q_TDATA "=all"


/* Simple list of callbacks */

#define COAP_N_BASE_CBR     (2)
#define COAP_N_SYS_CBR      (1)
#define COAP_N_LOG_CBR      (1)

#define COAP_MAX_CBR        (COAP_N_BASE_CBR + COAP_N_SYS_CBR + COAP_N_LOG_CBR)

struct coap_cb_reg coap_registry[COAP_MAX_CBR];
static int coap_reg_size;

error_t coap_uri_register(const char *path, coap_cb cbfunc, const char *corelink)
{
    int idx;

    if (coap_reg_size < COAP_MAX_CBR) {
        idx = coap_reg_size++;    
    } else {
        return ERR_NO_MEM;
    }
    
    coap_registry[idx].path = path;
    coap_registry[idx].cb = cbfunc;
    coap_registry[idx].link = corelink;
    
    return ERR_OK;
}


void coap_registry_init(void)
{
	/* Clear the registry */
	coap_reg_size = 0;

    /* mandatory elements */
    (void)coap_uri_register("", crtitle, "title=\"Info\";ct=0");
    (void)coap_uri_register(".well-known", crwellknown, NULL);

    /* basic resources from the NIC */
    (void)coap_uri_register(S_URI_SYSTEM, crsystem, CLA_SYSTEM);
}

/*** Dispatch request to registered handler ***/
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
static char id[] = {"Arduino MilliShield"}; // TODO: What ID is this?
static error_t crtitle(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp)
{
    char *d;
    if (req->code == COAP_REQUEST_GET) {
        d = (char*) m_append(rsp->msg, strlen(id));
        if (!d) {
            coap_stats.no_mbufs++;
            rsp->code = COAP_RSP_500_INTERNAL_ERROR;
        }
        else {
            memcpy(d, id, strlen(id));
            rsp->plen = strlen(id);
            rsp->cf = COAP_CF_TEXT_PLAIN;
            rsp->code = COAP_RSP_205_CONTENT;
            /* final as per request */
        }            
    }
    else {
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




/*
 * Do we want to handle unexpected options, like COAP_OPTION_URI_QUERY here,
 * and return 4.03 is appropriate, for example?
 */
static error_t crsystem_time(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it)
{
    struct optlv *o;
    uint32_t now;

    /* No URI path beyond /time is supported, so reject if present. */
    if ((o = copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it))) {
        rsp->code = COAP_RSP_404_NOT_FOUND;
        goto err;
    }            

    now = get_rtc_epoch();
   
    if (req->code == COAP_REQUEST_GET) {
        void *d;
        int len;
        now = htonl(now);
        len = sizeof(now);
        d = m_append(rsp->msg, len);
        if (!d) {
            coap_stats.no_mbufs++;
            return ERR_NO_MEM;
        }
        memcpy(d, &now, len);
        rsp->plen = len;
        rsp->cf = COAP_CF_APPLICATION_OCTET_STREAM;
        rsp->code = COAP_RSP_205_CONTENT;
    } else if (req->code == COAP_REQUEST_PUT) {
        m_adj(req->msg, req->hdrlen);
        coap_sys_time_data_t *td = mtod(req->msg, coap_sys_time_data_t *);

        /* Ensure type and length correct */
        if ((req->msg->m_pktlen == 0) || ((td->tl.u.rdt != crdt_time_abs) && 
                    (td->tl.u.rdt != crdt_time_delta)) || 
            (td->tl.l != 
                (sizeof(coap_sys_time_data_t) - sizeof(coap_sens_tl_t)))) {
            rsp->code = COAP_RSP_406_NOT_ACCEPTABLE;
        } else if (td->tl.u.rdt == crdt_time_abs) {
            // TODO: how to implement this?
            //if (rtc_set_time(ntohl(td->sec), ntohl(td->msec)) == ERR_OK)
            if (1)
            {
                dlog(LOG_DEBUG, "Time changed %lu.%lu", ntohl(td->sec), 
                        ntohl(td->msec));
                rsp->code = COAP_RSP_204_CHANGED;
            } else {
                /* Handle delta here */
                rsp->code = COAP_RSP_406_NOT_ACCEPTABLE;
            }
        }
    } else {
        rsp->code = COAP_RSP_501_NOT_IMPLEMENTED;
    }
    
    return ERR_OK;
err:
    rsp->plen = 0;

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




/*
 * Do we want to handle unexpected options, like COAP_OPTION_URI_QUERY here,
 * and return 4.03 is appropriate, for example?
 */
static error_t crsystem(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp)
{
    struct optlv *o;
    void *it = NULL;

    /* No observes on system */
    if ((o = copt_get_next_opt_type((const sl_co*)&(req->oh), 
                                    COAP_OPTION_OBSERVE, NULL))) {
        copt_del_opt_type((sl_co*)&(rsp->oh), COAP_OPTION_OBSERVE);
    }
    /* 
     * No URI path beyond /system, except /time and /stats is supported, so
     * reject if present. 
     */
    copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it);
    if ((o = copt_get_next_opt_type((const sl_co*) &(req->oh), COAP_OPTION_URI_PATH, &it))) {
        if (!coap_opt_strcmp(o, S_TIME_URI)) {
            return crsystem_time(req, rsp, it);
        } else if (!coap_opt_strcmp(o, S_STAT_URI)) {
            /* placeholder for stats */
            return crsystem_stats(req, rsp, it);
        } else {
            rsp->code = COAP_RSP_404_NOT_FOUND;
        }
    } else {
        rsp->code = COAP_RSP_404_NOT_FOUND;
    }

    rsp->plen = 0;

    return ERR_OK;
}
