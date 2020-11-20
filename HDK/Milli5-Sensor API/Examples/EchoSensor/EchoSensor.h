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


#ifndef ECHOSENSOR_H_
#define ECHOSENSOR_H_


#include <Arduino.h>
#include "sapi_error.h"
#include "log.h"


//////////////////////////////////////////////////////////////////////////
//
// Sensor Type. This is the resource name of the sensor.
// It is the leaf of the sensor URI. For example: /snsr/arduino/echo
//
// A Sensor Type has a max of 8 characters. Avoid using characters ,.;:{}-+*&%$#@!?<>|\/[]~`
//
//////////////////////////////////////////////////////////////////////////

// Echo sensor resource name
#define ECHO_SENSOR_TYPE		"echo"


/*
 * @brief Initialize th echo sensor. Callback called by sapi_init_sensor function.
 *
 * @return SAPI Error Code
 */
sapi_error_t echo_init_sensor();


/*
 * @brief Read the echo sensor. Builds and returns the payload. Callback called on
 *   CoAP GET sensor value
 *   CoAP Observe notification of sensor value
 *
 * @param  payload     Pointer to the sensor payload buffer.
 * @param  len         Pointer to the CoAP message buffer length.
 * @return SAPI Error Code
 */
sapi_error_t echo_read_sensor(char *payload, uint8_t *len);


/*
 * @brief Write sensor configuration. Processes payload sent from client. Callback called on
 *   CoAP PUT configuration value
 *
 * @param  payload     Pointer to the sensor payload buffer.
 * @param  len         Pointer to the CoAP message buffer length.
 * @return SAPI Error Code
 */
sapi_error_t echo_write_cfg(char *payload, uint8_t *len);


#endif /* ECHOSENSOR_H_ */