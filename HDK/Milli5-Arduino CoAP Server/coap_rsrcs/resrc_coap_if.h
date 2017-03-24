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

#ifndef _COAP_RESOURCE_IF_H_
#define _COAP_RESOURCE_IF_H_

#include "coapmsg.h"
#include "errors.h"

// Arduino sensors
#define L_URI_ARDUINO "arduino"
#define CLA_ARDUINO   "if=" "\"" L_URI_ARDUINO "\"" ";title=\"Arduino Sensors\";ct=42;"

/**
 * @brief CoAP Server Request/Response callback function
 *
 * @param[in] req CoAP Request
 * @param[in] rsp CoAP Response
 * @return error_t
 */
error_t crarduino(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp);

/**
 * @brief Create a CoAP Response message
 *
 * @return error_t
 */
error_t rsp_msg( struct mbuf * m, uint8_t *len, uint32_t count, float * reading, char * unit );

/**
 * @brief Init the Arduino resources such as sensors, LEDs etc.
 *
 * @return error_t
 */
error_t arduino_init_resources();

/**
 * @brief Enable Arduino sensors
 *
 * @return error_t
 */
error_t arduino_enab_sensors(void);

/**
 * @brief Disable Arduino sensors
 *
 * @return error_t
 */
error_t arduino_disab_sensors(void);

#endif /* _COAP_RESOURCE_IF_H_ */
