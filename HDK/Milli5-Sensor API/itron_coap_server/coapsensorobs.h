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

#ifndef INC_COAP_SENSOR_OBS_H
#define INC_COAP_SENSOR_OBS_H

#include "arduino.h"
#include "errors.h"
#include "hbuf.h"


#define MAX_OBSERVE_URI_LENGTH 32


/**
 * @brief A function pointer to set the sensor function used during Observe
 *
 * 
 */
typedef error_t (*ObsFuncPtr)(struct mbuf *, uint8_t *, uint8_t sensor_id);


/**
 * @brief Observe Registration Info struct
 *
 */
typedef struct observe_reg_info
{
	char			obs_uri[MAX_OBSERVE_URI_LENGTH];	// Sensor Device Type
	ObsFuncPtr				pObsFunc;					// Sensor Observe Handler callback
	time_t					base_epoch;
	uint32_t				frequency;					// Observation polling frequency (seconds)
	uint32_t				ack_seqno;					// Sequence number used in notification acks
	uint8_t					obs_flag;					// 1 -> observer has registered
	uint8_t					sensor_id;					// SAPI sensor id (0 used for backward compatibility)
} observe_reg_info_t;


// Observation Frequency
//
// When observations are enabled, the amount of time between observation notifications is
// determined by this define. The observation frequency is the number of seconds between
// observation notifications.
//
// The value is in seconds.
//   60   -> 1m
//   600  -> 10m
//   1800 -> 30m
//
#define OBSERVATION_FREQUENCY  60


/**
 * @brief Set the URI and attributes needed for generating observation notifications
 *   (used for obtaining token etc in CoAP Observe response msg). Called from SAPI.
 *
 * 
 */
uint8_t set_observer_sapi(const char * uri, ObsFuncPtr p, uint32_t frequency, uint8_t sensor_id);

/**
* @brief Set the URI and attributes needed for generating observation notifications
*   (used for obtaining token etc in CoAP Observe response msg).
 *
 * 
 */
void set_observer(const char * uri, ObsFuncPtr p);

/**
 * @brief Checks if Observe is turned on and if so, checks if a response should be sent
 *
 * @return boolean Returns a boolean that tells whether or not Observe is turned on
 */
boolean do_observe();

/**
 * @brief CoAP Register Observer. Called by SAPI.
 *
 * @return error_t
 */
error_t coap_obs_reg_sapi(uint8_t observer_id);

/**
* @brief CoAP Register Observer.
 *
 * @return error_t
 */
error_t coap_obs_reg();

/**
 * @brief CoAP De-register Observer. Called by SAPI.
 *
 * @return error_t
 */
error_t coap_obs_dereg_sapi(uint8_t observer_id);

/**
 * @brief CoAP De-register Observer.
 *
 * @return error_t
 */
error_t coap_obs_dereg();

/**
 * Handle CoAP ACK received.
 */
error_t observe_rx_ack(void *cbctx, struct mbuf *m);

/**
 * @brief CoAP Observe response.
 *
 * @return error_t
 */
error_t coap_observe_rsp(uint8_t observer_id);

#endif
