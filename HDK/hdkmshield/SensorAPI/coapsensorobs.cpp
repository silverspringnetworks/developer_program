/*

Copyright (c) Itron, Inc. 
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
#include "coapsensoruri.h"
#include "portability.h"
#include "arduino_time.h"


// Used to tell CoAP Server to use the SAPI dispatcher and handler
extern uint8_t is_sapi;


// Sensor Registration Info
observe_reg_info_t observe_info[MAX_OBSERVERS];

// Next empty slot in the observe info table
static uint8_t observe_info_index = 0;

// URL Classifier
extern char		classifier[CLASSIFIER_MAX_LEN];


/*
 * pending_rsp is the next payload to send to the proxy. It may be an observe
 * response that is awaiting a data link layer connection, or it could be the
 * last frame that was sent by us. In the latter case, we hold onto it to
 * resend in the event that the proxy/primary didn't receive it.
 *
 * This assumes we only have a data link window size of 1.
 */
struct mbuf *pending_rsp;



// This function assembles an URI and sets the function used to read a sensor
uint8_t set_observer_sapi(const char *sensor_type, ObsFuncPtr p, uint32_t frequency, uint8_t sensor_id)
{
	// Assemble the full resource URI; e.g. "/arduino/temp". Link to master observe table.
	sprintf(observe_info[observe_info_index].obs_uri, "/%s/%s", classifier, sensor_type);
	
	observe_info[observe_info_index].pObsFunc = p;
	observe_info[observe_info_index].frequency = frequency;
	observe_info[observe_info_index].sensor_id = sensor_id;
	observe_info[observe_info_index].obs_flag = 0;
	observe_info[observe_info_index].ack_seqno = 0;
	observe_info[observe_info_index].base_epoch = get_rtc_epoch();
	
	return observe_info_index++;
}


// This function assembles an URI and sets the function used to read a sensor.
// Supports backward compatibility.
void set_observer( const char *uri_rsrc_name, ObsFuncPtr p )
{
	// Assemble the full resource URI; e.g. "/arduino/temp". Link to master observe table.
	sprintf(observe_info[observe_info_index].obs_uri, "/arduino/%s", uri_rsrc_name);
	
	observe_info[observe_info_index].pObsFunc = p;
	observe_info[observe_info_index].frequency = OBSERVATION_FREQUENCY;
	observe_info[observe_info_index].sensor_id = 0;
	observe_info[observe_info_index].obs_flag = 0;
	observe_info[observe_info_index].ack_seqno = 0;
	observe_info[observe_info_index].base_epoch = get_rtc_epoch();
}


// Check each registered sensor. Determine if it we should generate an observation notification.
// Based on the periodic frequency defined for the sensor at registration time.
boolean do_observe()
{
	boolean atleastone = true;
	time_t  epoch      = get_rtc_epoch();
	
	// Check all registered observers
	for (uint8_t indx=0 ; indx < observe_info_index ; indx++)
	{
		if (observe_info[indx].obs_flag)
		{
			if (observe_info[indx].base_epoch == 0)
			{
				observe_info[indx].base_epoch = epoch;
			}
			
			// If frequency is zero - skip doing Observe Response
			if (observe_info[indx].frequency == 0)
			{
				continue;
			}
		
			// Check if the current epoch is greater then frequency interval
			if (epoch >= (observe_info[indx].base_epoch+observe_info[indx].frequency))
			{
				// Record the current minute
				dlog(LOG_DEBUG, "do_observe: epoch %x uri %s", observe_info[indx].base_epoch, observe_info[indx].obs_uri);
				observe_info[indx].base_epoch = epoch;

				// Generate and send response notification
				coap_observe_rsp(indx);
				atleastone = true;
			
				int freeram = free_ram();
				dlog(LOG_DEBUG, "do_observe: Free Ram: %d", freeram);
				break;
			}
		}
	}
	
	// Return the delay for reading time flag
	return atleastone;
	
}


// Register for Observe used by SAPI handlers.
error_cs_t coap_obs_reg_sapi(uint8_t observer_id)
{
	// Record the minute that we turn on Observe
	// Make sure we don't send Observe response more than once per minute
	observe_info[observer_id].base_epoch = get_rtc_epoch();
	
	// Flag that we are doing Observe
	observe_info[observer_id].obs_flag = 1;
	
	// Set start sequence number (must be non-zero)
	observe_info[observer_id].ack_seqno = 10;

	// Set mNIC wake-up pin to HIGH, so that we can toggle it 0 -> 1
	//pinMode(MNIC_WAKEUP_PIN,OUTPUT);
	//digitalWrite(MNIC_WAKEUP_PIN,HIGH);

	return ERR_OK;
}


// Register for Observe. Used for backward compatibility. Observer Id is 0 for backward compatibility.
error_cs_t coap_obs_reg()
{
	// Record the minute that we turn on Observe
	// Make sure we don't send Observe response more than once per minute
	observe_info[0].base_epoch = get_rtc_epoch();
	
	// Flag that we are doing Observe
	observe_info[0].obs_flag = 1;
	
	// Set start sequence number (must be non-zero)
	observe_info[0].ack_seqno = 10;

	// Set mNIC wake-up pin to HIGH, so that we can toggle it 0 -> 1
	pinMode(MNIC_WAKEUP_PIN,OUTPUT);
	digitalWrite(MNIC_WAKEUP_PIN,HIGH);

	return ERR_OK;
}


// Register for Observe used by SAPI handlers.
error_cs_t coap_obs_dereg_sapi(uint8_t observer_id)
{
	// Flag that we are not doing Observe
	observe_info[observer_id].obs_flag = 0;
	observe_info[observer_id].base_epoch = 0;
	observe_info[observer_id].ack_seqno = 0;

	dlog(LOG_DEBUG, "De-register Observe: %d", observer_id);
	return ERR_OK;

}


// De-Register for Observe. Used for backward compatibility. Observer Id is 0 for backward compatibility.
error_cs_t coap_obs_dereg()
{
	// Flag that we are not doing Observe
	observe_info[0].obs_flag = 0;
	
	// Set mNIC wake-up pin to LOW
	digitalWrite(MNIC_WAKEUP_PIN,LOW);

	dlog(LOG_DEBUG, "De-register Observe: 0");
	return ERR_OK;
}


/*
 * Handle CoAP ACK received.
 */
error_cs_t observe_rx_ack(void *cbctx, struct mbuf *m)
{
	// Bump the sequence number.
	uint32_t seq_number = *((uint32_t *)cbctx);
	seq_number++;
	*((uint32_t *)cbctx) = seq_number;
	
	return ERR_OK;
}


// Generate CoAP Observe response message
/*
 * Find the correct entry in the obs array. If not present, just return.
 * Allocate a RSP context.
 * Initialize the context.
 * Set the RSP type to CON.
 * Set the code and plen, if required.
 * coap_msg_response() to build a response.
 * Register for callback when ACK received.
 * Set pending_rsp, freeing any existing one first.
 */
error_cs_t coap_observe_rsp(uint8_t observer_id)
{
	struct coap_msg_ctx rsp;
    coap_ack_cb_info_t 	cbi;			// Callback info
    uint8_t 			len = 0;		// Message length
    uint8_t 			nxt = 0;		// The next observer
    struct mbuf *		m = NULL;		// Observe response message
    struct optlv 		opt;
    error_cs_t 			rc = ERR_OK;

	// Safety check. If a message is already in progress do nothing.
    if (pending_rsp) 
	{
		dlog(LOG_DEBUG, "coap_observe_rsp: Pending response in progress - skipping new notification");
		return ERR_INPROGRESS;
    }
	
	// Clear CoAP response message
	memset(&rsp, 0, sizeof(rsp));
	
	// Get token from observers (obs) table
	rc = get_obs_by_uri(observe_info[observer_id].obs_uri, &(rsp.tkl), rsp.token, &(rsp.client), &nxt);
    if (rc)
    {
        dlog(LOG_ERR, "get_obs_by_uri failed: %s", observe_info[observer_id].obs_uri);
        return ERR_NO_ENTRY;
    }
    
    // Initialize CoAP response message options
    copt_init((sl_co*)&(rsp.oh));

    // Allocate an mbuf
	m = m_gethdr();
    if (!m) 
	{
		rc = ERR_NO_MEM;
        goto error;
    }
	
    /* Allow room for CoAP header */
    m_prepend(m, COAP_OBS_HDR_SZ);

    // Ask sensor code for a reading (that is read the sensor and return a CoAP response.
	if (is_sapi == 1)
	{
		rc = (*(observe_info[observer_id].pObsFunc))(m, &len, observe_info[observer_id].sensor_id);
	}
	else
	{
		rc = (*(observe_info[observer_id].pObsFunc))(m, &len, 0);
		//rc = (*pObsFunc)(m, &len);
	}
    if (rc != ERR_OK) 
	{
        //m_free(m);
        goto error;
    }

    /* Move payload up to start of buffer, enough space to pre-pend l8r. */
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
	rsp.cf = COAP_CF_APPLICATION_CBOR;
    rsp.type = COAP_T_NCONF_VAL; // TODO: CON or NON?

    /*
     * Build actual CoAP response message. Will be sent over HDLC link later.
     */
    if (coap_msg_response(&rsp) != ERR_OK) 
	{
        dlog(LOG_ERR, "coap_observe_rsp: Error creating response");
        //m_free(m);
        goto error;
    }

    /*
     * Record the next sequence number we'll use for notification.
	 * When acked, we'll ack this number, indicating that's what next.
     */
    cbi.cbctx = (void *) &observe_info[observer_id].ack_seqno;
    
    /*
     * Set callback and register for notification of ACK.
     */
    cbi.cb = observe_rx_ack;
    coap_con_add(rsp.mid, &cbi);

    /*
     * If shared transfer buffer has prior response than free it and post the new response.
     */
    if (pending_rsp) 
	{
        m_free(pending_rsp);
		dlog(LOG_DEBUG, "%s:%d Cleared pending_rsp", __FUNCTION__, __LINE__);
    }
    pending_rsp = rsp.msg;
    copt_del_all((sl_co*)&(rsp.oh));

	/* Notify Itron NIC of observe response request */
	digitalWrite(MNIC_WAKEUP_PIN,LOW);
	delay(MILLI_WAKE_DELAY);
	digitalWrite(MNIC_WAKEUP_PIN,HIGH);
    return ERR_OK;

error:
    copt_del_all((sl_co*)&(rsp.oh));
    dlog(LOG_DEBUG, "coap_observe_rsp: free response mbuf on error");
    m_free(m);
    return rc;
}

