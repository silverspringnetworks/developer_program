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

#ifndef TEMPSENSOR_H_
#define TEMPSENSOR_H_


#include <Arduino.h>
#include <DHT_U.h>
#include "sapi_error.h"
#include "arduino_time.h"
#include "log.h"


//////////////////////////////////////////////////////////////////////////
//
// Sensor Type. Think of this as the resource name of the sensor.
// It is the leaf of the sensor URI. For example: /snsr/arduino/temp
//
// A Sensor Type has a max of 8 characters. Avoid using characters ,.;:{}-+*&%$#@!?<>|\/[]~`
//
//////////////////////////////////////////////////////////////////////////

// DHT11 temp sensor type
#define TEMP_SENSOR_TYPE		"temp"


/*
 * @brief Initialize DHT11 temp sensor. Callback called by sapi_init_sensor function.
 *
 * @return SAPI Error Code
 */
sapi_error_t temp_init_sensor();


/*
 * @brief Reads a DHT11 sensor. Builds and returns the payload. Callback called on
 *   CoAP Get sensor value
 *   CoAP Observe notification of sensor value
 *
 * @param payload     Pointer to the sensor payload buffer.
 * @param len         Pointer to the CoAP message buffer length.
 * @return SAPI Error Code
 */
sapi_error_t temp_read_sensor(char *payload, uint8_t *len);


/*
 * @brief Read sensor configuration. Builds and returns the payload. Callback called on
 *   CoAP Get configuration value
 *
 * @param payload     Pointer to the sensor payload buffer.
 * @param len         Pointer to the CoAP message buffer length.
 * @return SAPI Error Code
 */
sapi_error_t temp_read_cfg(char *payload, uint8_t *len);


/*
 * @brief Write sensor configuration. Processes payload sent from client. Callback called on
 *   CoAP Put configuration value
 *
 * @param payload     Pointer to the sensor payload buffer.
 * @param len         Pointer to the CoAP message buffer length.
 * @return SAPI Error Code
 */
sapi_error_t temp_write_cfg(char *payload, uint8_t *len);


/*
 * @brief Code to build the sensor payload. Temp payload is text with this format:
 *   <epoch>,<temp>,<uom>
 *     <epoch> is a 4 byte (8 hex char) timestamp
 *     <temp> is a decimal number
 *     <uom> is a char: C or F
 *
 *  Note that the payload is text. Payloads can also be a byte array of binary data
 *
 * @param buf     Pointer to the payload buffer.
 * @param reading Pointer to the season reading.
 * @return SAPI Error Code
 */
sapi_error_t temp_build_payload(char *buf, float *reading);


/*
 * @brief Read the temp value from the DHT11 sensor.
 *
 * @param reading     Pointer to a float to contain the value.
 * @return SAPI Error Code
 */
sapi_error_t read_dht11(float *reading);


/*
 * @brief Enable the sensor in its context.
 *
 * @return SAPI Error Code
 */
sapi_error_t temp_sensor_enable(void);


/*
 * @brief Disable the sensor in its context.
 *
 * @return SAPI Error Code
 */
sapi_error_t temp_sensor_disable(void);


// Max temperature of the DHTxx series is 80.0C. If no sensor we return 100.0C.
#define NO_SENSOR_TEMP		100.0


// Scale (Unit of Measure) related
typedef enum
{
	INVALID_TEMP		= -100,
	CELSIUS_SCALE		= 0x43,
	FAHRENHEIT_SCALE	= 0x46,
	NUMBER_OF_SCALES	= 2
} temp_scale_t;

// Temp Sensor Alert Type related
typedef enum {
	tsat_disabled = 0,
	tsat_no_change,
	tsat_cleared,
	tsat_high,
	tsat_low
} temp_sensor_alert_t;

// Temp Sensor Contet Type
typedef struct temp_ctx
{
	temp_scale_t		scalecfg;
	temp_sensor_alert_t alertstate;
	int8_t				temp_max_thres;
	int8_t				temp_min_thres;
	uint8_t				enable;
} temp_ctx_t;


#endif /* TEMPSENSOR_H_ */