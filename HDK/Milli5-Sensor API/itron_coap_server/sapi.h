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

#ifndef SAPI_H_
#define SAPI_H_

#include <Arduino.h>
#include "sapi_error.h"


//////////////////////////////////////////////////////////////////////////
//
// SAPI Callback Typdefs
//
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Typedef sensor initialization function pointer.
 *
 * Callback by SAPI from sapi_init_sensor. It should contain any code needed to initialize the
 * hardware sensor and the related sensor code.
 *
 * @return SAPI Error Code.
 */
typedef sapi_error_t (*SensorInitFuncPtr)();

/**
 * @brief Typedef sensor read callback function pointer.
 *
 * Callback by SAPI in response to a CoAP GET "sens" request.
 *
 * @param payload Char pointer to the payload. Copy the sensor payload to be returned.
 * @param len     Pointer to the payload length. Set the sensor payload length to be returned.
 * @return SAPI Error Code.
 */
typedef sapi_error_t (*SensorReadFuncPtr)(char *payload, uint8_t *len);

/**
 * @brief Typedef sensor read configuration callback function pointer.
 *
 * Callback by SAPI in response to a CoAP GET "cfg" request.
 *
 * @param payload Char pointer to the payload. Copy the config payload to be returned.
 * @param len     Pointer to the payload length. Set the config payload length to be returned.
 * @return SAPI Error Code.
 */
typedef sapi_error_t (*SensorReadCfgFuncPtr)(char *payload, uint8_t *len);

/**
 * @brief Typedef sensor write configuration callback function pointer.
 *
 * Callback by SAPI in response to a CoAP PUT "cfg" request.
 *
 * @param payload Char pointer to the payload. Process the payload to extract configuration.
 * @param len     Pointer to the payload length. Length of the config payload.
 * @return SAPI Error Code.
 */
typedef sapi_error_t (*SensorWriteCfgFuncPtr)(char *payload, uint8_t *len);


//////////////////////////////////////////////////////////////////////////
//
// SAPI Framework Functions
//
//////////////////////////////////////////////////////////////////////////

/**
 * @brief SAPI Initialization.
 *
 * Call this function first in your init initialization code (after the bootstrap code).
 * This function will initialize the underlying CoAP Server, the RTC, the SAPI framework
 * and will send the milli a reboot message.
 *
 * The classifier is used to identify the first level CoAP request handler. A sensor URL is composed of 
 * a {prefix} like "snsr", a {classifier} like "arduino", and a sensor resource like "temp".
 * A typical URL would look like this: /snsr/arduino/temp.
 * The {prefix} is stripped off by the Milli CoAP Proxy. 
 *
 * @param url_classifier	URL classifier string. If NULL the default is "arduino". Max length 16 characters.
 */
void sapi_initialize(char *url_classifier);

/**
 * @brief Idle loop run.
 *
 * Call in your sensors "idle" or "main" loop. Needed to make sure that the underlying
 * CoAP server can processor incoming messages from the milli and that notification payloads
 * are pushed up.
 */
void sapi_run();


//////////////////////////////////////////////////////////////////////////
//
// SAPI sensor functions
//
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Register a sensor.
 *
 * Use this function to register sensor. Usually call this in your initialization code (after the bootstrap code).
 * Your sensor code needs to provide a number of callback functions. A sensor package can support any number of sensors.
 *
 * Note: 4 sensors max are supported by default (that is you can make 4 registration calls).
 * Note: At this time the CoAP Server will only support one sensor for observations.
 *
 * @param sensor_type     Sensor device type. For example "temp". Used in client URI's and MQTT Topic Names.
 * @param sensor_init     Pointer to the sensor initialization callback function.
 * @param sensor_read     Pointer to the read sensor callback function.
 * @param sensor_readcfg  Pointer to the read configuration callback function. If not supported set to NULL.
 * @param sensor_writecfg Pointer to the write configuration callback function. If not supported set to NULL.
 * @param is_observer     Set to 1 if this sensor is to generate observation notifications. Set to 0 if not.
 * @param frequency       Periodic observation generation frequency (in seconds).
 * @return Sensor Id.
 */
uint8_t sapi_register_sensor(char *sensor_type, SensorInitFuncPtr sensor_init, SensorReadFuncPtr sensor_read, SensorReadCfgFuncPtr sensor_readcfg,
							 SensorWriteCfgFuncPtr sensor_writecfg, uint8_t is_observer, uint32_t frequency);

/**
 * @brief Initialize a sensor (hardware) and sensor related code.
 *
 * Use this function to do any initialization your sensor code and sensor hardware required.
 * Note that this function will call the sensor init function provided to the sapi_register_sensor function.
 *
 * @param sensor_id Id of the sensor to initialize (returned by sapi_register_sensor).
 * @return SAPI Error Code
 */
sapi_error_t sapi_init_sensor(uint8_t sensor_id);


//////////////////////////////////////////////////////////////////////////
//
// SAPI notification functions
//
//////////////////////////////////////////////////////////////////////////

/**
 * @brief Function used to push a notification.
 *
 * Use this function when your sensor (and code) determines that a message (aka alert) needs to be pushed
 * up to an "application" for processing.
 *
 * @param sensor_id Id of the sensor to generate an observation notification for.
 * @return SAPI Error Code
 */
sapi_error_t sapi_push_notification(uint8_t sensor_id);


#endif /* SAPI_H_ */