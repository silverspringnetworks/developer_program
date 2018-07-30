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



#include "errors.h"
#include "log.h"
#include "coappdu.h"
#include "coapmsg.h"
#include "coapobserve.h"
#include "coapextif.h"
#include "coapsensorobs.h"
#include "temp_sensor.h"
#include "arduino_pins.h"
#include "arduino_time.h"


// The most recent minute we sent a CoAP response message
// Make sure we don't send Observe response more than once per minute
static uint32_t prev_minute = 0;
static boolean	obs_flag = false;

// Check if we should send Observe message
boolean do_observe()
{
	// Check if we are doing Observe
	if (obs_flag)
	{
		uint32_t m = rtc.getMinutes();
		
		// Check if the current minute is different from the previous minute
		if ( m != prev_minute )
		{
			// Record the current minute
			prev_minute = m;

			// Send response
			coap_observe_rsp();

		} // if
	} // if
	
	// Return the obs_flag
	return obs_flag;
	
} // do_observe

// Sequence number 
// TODO: Don't know yet how it is used
static uint32_t start_sn; 

// Register for Observe
error_t coap_obs_reg()
{
	// Record the minute that we turn on Observe
	// Make sure we don't send Observe response more than once per minute
	prev_minute = rtc.getMinutes();
	
	// Flag that we are doing Observe
	obs_flag = true;

	// Set mNIC wake-up pin to HIGH, so that we can toggle it 0 -> 1
	pinMode(MNIC_WAKEUP_PIN,OUTPUT);
	digitalWrite(MNIC_WAKEUP_PIN,HIGH);

	// Set start sequence number
	start_sn = 10; // Non-zero value

	return ERR_OK;

} // coap_obs_reg

// De-register for Observe
error_t coap_obs_dereg()
{
	// Quit Observe
	println("De-register for Observe");
	obs_flag = false;
	
	// Set mNIC wake-up pin to LOW
	digitalWrite(MNIC_WAKEUP_PIN,LOW);

	return ERR_OK;

} // coap_obs_dereg

/*
 * Handle CoAP ACK received.
 */
error_t observe_rx_ack( void *cbctx, struct mbuf *m )
{
	start_sn++;
	return ERR_OK;
}
/*
 * Find the correct entry in the obs array. If not present, just return.
 * Allocate a RSP context.
 * Initialise the context.
 * Set the RSP type to CON.
 * Set the code and plen, if required.
 * coap_msg_response() to build a response.
 * Register for callback when ACK received.
 * Set pending_rsp, freeing any existing one first.
 */

/*
 * pending_rsp is the next payload to send to the proxy. It may be an observe
 * response that is awaiting a datalink layer connection, or it could be the
 * last frame that was sent by us. In the latter case, we hold onto it to
 * resend in the event that the proxy/primary didn't receive it.
 *
 * This assumes we only have a datalink window size of 1.
 *
 * if r is NULL, the call is just to determine if there's an outstanding
 * observe.
 */
struct mbuf *pending_rsp;

#define MAX_OBSERVE_URI_LENGTH 32
// This array will contain the URI used to obtain Token etc for the response
static char 			obs_uri[MAX_OBSERVE_URI_LENGTH];
// This function will be used to read a sensor and obtain the response message
static ObsFuncPtr		pObsFunc;

// This function assmebles an URI and sets the function used to read a sensor
void set_observer( const char * uri, ObsFuncPtr p )
{
	// Assemble the URI, e.g. "/arduino/temp"
	sprintf( obs_uri, "/arduino/%s", uri );
	
	// Set the pointer to the Observe function
	pObsFunc = p;

} // set_observer()

// Generate Observe response message
error_t coap_observe_rsp()
{
	uint8_t 			len;			// Length of temperature message
    coap_ack_cb_info_t 	cbi;			// Callback info
    struct mbuf *		m = NULL;		// Observe response message
    struct coap_msg_ctx rsp;
    uint8_t 			nxt = 0;		// The next Observer
    struct optlv 		opt;
    error_t 			rc = ERR_OK;

	// Clear CoAP message
	memset(&rsp, 0, sizeof(rsp));
	
	// Init CoAP options
    copt_init((sl_co*)&(rsp.oh));
	
	// Get token etc.
	rc = get_obs_by_uri( obs_uri, &(rsp.tkl), rsp.token, &(rsp.client), &nxt);
    if (rc)
    {
        dlog(LOG_ERR, "get_obs_by_uri failed\n");
        rc = ERR_NO_ENTRY;
        goto error;
    }

    // Allocate an mbuf
	m = m_gethdr();
    if (!m) 
	{
        goto done;
    }
	
    /* Allow room for coap header */
    m_prepend( m, COAP_OBS_HDR_SZ );

    // Get temperature reading
	rc = (*pObsFunc)(m,&len);
    if (rc) 
	{
        m_free(m);
        goto done;
    }

    /* Move payload up to start of buffer, enough space for prepend l8r. */
    m_adj( m, COAP_OBS_HDR_SZ );
    rsp.msg = m;
	
	// Add Message ID
    rsp.mid = get_mid_val();

	// Add Observe option
	opt.ot = COAP_OPTION_OBSERVE;
	opt.ol = 3;
	if (copt_add_opt((sl_co*)&(rsp.oh), &opt) != ERR_OK) 
	{
		dlog(LOG_ERR, "Couldn't add Observe option");
		rc = ERR_NO_MEM;
		goto error;
	}

	// Add Max-Age option
	opt.ot = COAP_OPTION_MAXAGE;
	opt.ol = 4;
	if (copt_add_opt((sl_co*)&(rsp.oh), &opt) != ERR_OK) 
	{
		dlog(LOG_ERR, "Couldn't add Max-Age option");
		rc = ERR_NO_MEM;
		goto error;
	}

    /*
     * Now get the content. Does the m_append to the mbuf.
     */
	rsp.plen = m->m_pktlen; /* payload includes type and length */
    rsp.code = COAP_RSP_205_CONTENT;
	rsp.cf = COAP_CF_CSV;
    rsp.type = COAP_T_NCONF_VAL; // TODO: CON or NON?

    /*
     * Setup msg hdr for transmission.
     */
    if (coap_msg_response(&rsp) != ERR_OK) 
	{
        dlog(LOG_ERR, "Error creating observe RSP");
        m_free(m);
        goto error;
    }

    /*
     * Record the next sn we'll use for notification. i.e. when acked,
     * we'll ack this number, indicating that's what next.
     */
    cbi.cbctx = (void *) start_sn;
    
    /*
     * Register for notification of ACK.
     */
    cbi.cb = observe_rx_ack;
    coap_con_add(rsp.mid, &cbi);

    /*
     * Save it for sending later.
     */
    if (pending_rsp) 
	{
        m_free(pending_rsp);
		dlog(LOG_DEBUG, "%s:%d Cleared pending_rsp", __FUNCTION__, __LINE__);
    }
    pending_rsp = rsp.msg;
    copt_del_all((sl_co*)&(rsp.oh));

	/* Notify mnic of observe request, wait for 1ms, then high again */
	digitalWrite(MNIC_WAKEUP_PIN,LOW);
	delay(1);
	digitalWrite(MNIC_WAKEUP_PIN,HIGH);
    return ERR_OK;

done:
error:
    copt_del_all((sl_co*)&(rsp.oh));
    m_free(m);

    return rc;
	
} // coap_observe_rsp()

