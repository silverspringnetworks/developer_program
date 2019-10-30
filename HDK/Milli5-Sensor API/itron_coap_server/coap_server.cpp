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

#include <arduino.h>

#include "hbuf.h"
#include "hdlcs.h"
#include "log.h"
#include "errors.h"
#include "coappdu.h"
#include "coapextif.h"
#include "coapmsg.h"
#include "coapsensoruri.h"
#include "coapobserve.h"
#include "exp_coap.h"
#include "coap_server.h"


/* 
 * Use environment variable COAP_DATA_ROOT to set server data root dir 
 *
 * /                  hard coded general info message - incl. vendor
 * /.well-known/core  core.coap  map of resources
 * /time              UTC
 * 
 */


// CoAP Server initialization
void coap_s_init(HardwareSerial *pSerial, uint32_t max_age, uint32_t uart_timeout_ms, uint32_t max_hdlc_payload_size, const char *uri_rsrc_name, ObsFuncPtr pObsFuncPtr)
{
	int res;
	
	// Initialize CoAP Server registry
	coap_registry_init();

	// Set Max-Age: CoAP Server Response Option 14
	coap_set_max_age(max_age);
	
	// Set the URI used for obtaining token etc in CoAP Observe response msg
	set_observer(uri_rsrc_name, pObsFuncPtr);

	// Open the HDLC connection
	res = hdlcs_open(pSerial, uart_timeout_ms, max_hdlc_payload_size);
	if (res) 
	{
		dlog(LOG_ERR, "HDLC initialization failed!");
	}
	
	// That's all folks!
} // coap_s_init()


/*** limited to static data and time ***/

#define xstr(s)   str(s)
#define str(s)    #s

/*
 * Primary CoAP process function.
 * Set up REQ and RSP contexts.
 * Parse the REQ.
 * Deal with special case messages or errors, or hand off for URI servicing.
 * Enable or disable observe, as appropriate.
 * Build a response PDU based on the RSP context, and return the mbuf.
 */
mbuf_ptr_t coap_s_proc( mbuf_ptr_t m )
{
    struct coap_msg_ctx cc, rcc;
    void *clt = NULL;   /* Not used on sensor, 1 HDLC connection. */
    struct optlv *op;
    error_t rc;
    uint8_t code;
    char *pstr;
    
    /* Allocate response buffer */
    struct mbuf *r = NULL;
    r = m_get();
    if (r == NULL)
    {
	    goto done;
    }

    /* Parse incoming message */
    memset(&cc, 0, sizeof(cc));
    copt_init((sl_co*)&(cc.oh));
    memset(&rcc, 0, sizeof(rcc));
    copt_init((sl_co*)&(rcc.oh));
    rc = coap_msg_parse(&cc, m, &code);

    if (rc == ERR_OK)
	{
        if (cc.type == COAP_T_ACK_VAL)
		{
            /*
             * TODO: Assuming it's not a piggy-backed ACK for now.
             */
            rc = coap_ack_rx(cc.mid, NULL);
            dlog(LOG_INFO, "ACK for mid: 0x%x received, lookup returned %d", cc.mid, rc);
            rc = ERR_NORSP;
            goto done;
        }
        coap_init_rsp(&cc, &rcc, r);

        /* Currently the proxy is catching all empty msgs anyway... */
        if (cc.code == COAP_EMPTY_MESSAGE)
		{
            rcc.plen = 0;
            if (cc.type == COAP_T_CONF_VAL)
			{
                rcc.type = COAP_T_RESET_VAL;
            }
        }
		else
		{
            if (coap_s_uri_proc(&cc, &rcc) != ERR_OK)
			{
                rcc.code = COAP_RSP_500_INTERNAL_ERROR;
                rcc.plen = 0;
            }
        }
       
        /*
         * If this isn't the final message in this interaction, record the
         * relevant information. We could convert the options to a string, or
         * even just memcpy all the uripath options into the field. Extraneous
         * GETs shouldn't have obs set, so that shouldn't result in extraneous
         * enables, except that we're allowing them to replace existing
         * observes.
         * final will have been set if there was an error processing the URL or
         * the return code is not 2.*.
         */
        pstr = coap_pathstr(&cc);
        if (!rcc.final && copt_get_next_opt_type((sl_co*)&(rcc.oh), COAP_OPTION_OBSERVE, NULL))
		{
            (void)disable_obs(pstr, &cc, &clt, 1);
            if (enable_obs(pstr, &cc, &clt) != ERR_OK)
			{
                (void)copt_del_opt_type((sl_co*)&(rcc.oh), COAP_OPTION_OBSERVE);
                rcc.final = 1;
                dlog(LOG_ERR, "Failed to enabled observe for URI: %s", pstr);
            }
			else
			{
                dlog(LOG_DEBUG, "Enabled observe for URI: %s", pstr);
            }
        } else if ((op = copt_get_next_opt_type((sl_co*)&(cc.oh), COAP_OPTION_OBSERVE, NULL)) && 
				   (co_uint32_n2h(op) == COAP_OBS_DEREG))
		{
            /*
             * Could check for class 2 code before doing this, but this shouldn't hurt.
             * DEREG request. No need to call client_done as coap_proc is a
             * synchronous call, so the caller can handle it.
             */
            if (disable_obs(pstr, &cc, &clt, 0) == ERR_OK)
			{
                dlog(LOG_DEBUG, "Disabled observe for URI %s", pstr);
            }
        }

        if (coap_msg_response(&rcc) != ERR_OK)
		{
			if (r)
			{
				dlog(LOG_DEBUG, "Error msg rsp: freeing mbuf");
				m_free(r);
				r = NULL;
			}
            goto done;
        }

		/* No response */
        if(r->m_pktlen == 0)
		{
	        dlog(LOG_DEBUG, "No rsp: freeing mbuf");
            m_free(r);
            r = NULL;
        }

        /* hand back reply */
        /* START */    
    }
	else if (rc == ERR_VER_NOT_SUPP)
	{
        /*
         * Silently ignore.
         */
         if (r)
         {
			dlog(LOG_DEBUG, "Parse error: freeing mbuf");
			m_free(r);
			r = NULL;
         }
        rc = ERR_OK;
        goto done;
    }
	else
	{
        /*
         * There was some sort of issue with the request, build a response
         * that indicates the issue.
         */
        dlog(LOG_ERR, "Error: rc/h->len: %d/%d, cc.code: %d", rc, m->m_pktlen, cc.code);
        
        /*
         * Leave token length and token as-is.
         * code as per return from coap_msg_parse.
         * mid, as-is.
         * Set payload, plen, cf (content format) for the response.
         * Don't care about URI.
         * No observe.
         */
        coap_init_rsp(&cc, &rcc, r);
		
        if (cc.type == COAP_T_CONF_VAL)
		{
            rcc.type = COAP_T_ACK_VAL;
        }
		else
		{
            rcc.type = COAP_T_NCONF_VAL;
        }
        rcc.code = code;
        
        if (coap_msg_response(&rcc) != ERR_OK)
		{
			if (r)
			{
				dlog(LOG_DEBUG, "Error msg rsp: freeing mbuf");
				m_free(r);
				r = NULL;
			}
			goto done;
        }
    }

done:
    if (cc.msg)
	{
		dlog(LOG_DEBUG, "coap_s_proc: Free cc mbuf");
        m_free(cc.msg);
        cc.msg = NULL;
    }
    copt_del_all((sl_co*)&(cc.oh));
    copt_del_all((sl_co*)&(rcc.oh));
    return r;
	
}


// Run HDLCS and the CoAP Server 
void coap_s_run()
{
	struct mbuf *appd;
	struct mbuf *arsp;
	
	/* Run the secondary-station HDLC state machine */
	hdlcs_run();
	
	/* Serve incoming request, if any */
	appd = hdlcs_read();
	if (appd) 
	{
		/* Run the CoAP server */
		arsp = coap_s_proc(appd);
		if (arsp) 
		{
			// Direct send of CoAP response
			hdlcs_write(arsp->data, arsp->len);
			
			// Free response mbuf
			dlog(LOG_DEBUG, "coap_s_run: freeing arsp mbuf");
			m_free(arsp);
		}
		// Free request mbuf
		dlog(LOG_DEBUG, "coap_s_run: freeing appd mbuf");
		m_free(appd);
		
		int freeram = free_ram();
		dlog(LOG_DEBUG, "coap_s_run: free Ram: %d", freeram);
	}
} 
