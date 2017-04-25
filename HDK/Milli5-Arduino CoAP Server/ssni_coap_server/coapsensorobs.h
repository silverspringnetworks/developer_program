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


/**
 * @brief A function pointer to set the sensor function used during Observe
 *
 * 
 */
typedef error_t (*ObsFuncPtr)( struct mbuf *, uint8_t * );

/**
 * @brief Set the URI used for obtaining token etc in CoAP Observe response msg
 *
 * 
 */
void set_observer( const char * uri, ObsFuncPtr p );

/**
 * @brief Checks if Observe is turned on and if so, checks if a response should be sent
 *
 * @return boolean Returns a boolean that tells whether or not Observe is turned on
 */
boolean do_observe();

/**
 * @brief CoAP Register for Observe
 *
 * @return error_t
 */
error_t coap_obs_reg();

/**
 * @brief CoAP De-egister for Observe
 *
 * @return error_t
 */
error_t coap_obs_dereg();

/**
 * Handle CoAP ACK received.
 */
error_t observe_rx_ack( void *cbctx, struct mbuf *m );

/**
 * @brief CoAP Observe response
 *
 * @return error_t
 */
error_t coap_observe_rsp();

#endif
