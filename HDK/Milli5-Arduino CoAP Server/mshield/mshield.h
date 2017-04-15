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

#ifndef _MSHIELD_H_
#define _MSHIELD_H_

#include "Arduino.h"
#include "coap_server.h"
#include "coappdu.h"
#include "coapmsg.h"
#include "temp_sensor.h"
#include "log.h"
#include "arduino_time.h"


/******************************************************************************/
//
// List of sensors defined using strings, which will be part of the CoAP URI
//

// The default sensor for this Reference App is a temperature sensor (DHT11)
// To access this sensor, the URI is /sensor/arduino/temp
#define TEMP_SENSOR           			"temp"

/* Add your own sensors here using a string of max 22 characters              */
/* Avoid using characters such as ,.;:{}-+*&%$#@!?<>|\/[]~`                   */
/* The string below will be part of the CoAP URI used to access this sensor   */
/* If the string is "humi", the complete URI will be /sensor/arduino/humi     */
#define MY_SENSOR             			"a_sensor"
#define MY_SECOND_SENSOR       			"another_sensor"


/******************************************************************************/
//
// It's possible to do a CoAP Observe on one sensor
// Once a minute, your CoAP Client will receive a response message  
// containing sensor data with timestamp and unit
//

// Pick one sensor from the sensors above to make it an "observable" sensor:
#define OBS_SENSOR_NAME     			TEMP_SENSOR

// Specify the function that reads the sensor and assembles the 
// CoAP Observe response message:
#define OBS_FUNC_PTR        			&arduino_get_temp 


/******************************************************************************/
//
// CoAP Observe Max-Age, see Section 5.10.5 of rfc7252
//
#define COAP_MSG_MAX_AGE_IN_SECS		90


/******************************************************************************/
//
// Specify a pointer to the Serial object used for printing to the Serial Monitor
//

#if defined(ARDUINO_ARCH_SAMD)
  // Use SerialUSB for Serial Monitor on Zero based boards
  #define SER_MON_PTR					&SerialUSB
#endif

#if defined(ARDUINO_ARCH_SAM)
  #define SER_MON_PTR					&SerialUSB
#endif

/******************************************************************************/
//
// Specify baud rate for the Serial Monitor
//
#define SER_MON_BAUD_RATE     			115200

/******************************************************************************/
//
// Specify a pointer to the Serial object used to access the UART for the HDLC 
// connection
//
// NOTE: the baud rate of the HDLC connection is fixed
//
#define UART_PTR        				&Serial1


/******************************************************************************/
//
// Turn logging on or off
//   HIGH means logging is turned on
//   LOW  means logging is turned off

#define LOG_LEVEL						HIGH // LOW


/******************************************************************************/
//
// Set the time-out for the UART between Arduino and MilliShield
//
#define UART_TIMEOUT_IN_MS				2000


/******************************************************************************/
//
// Local time zone relative to UTC; Examples PST: -8, MST: -7, Central: -6, EST: -5
//
#define LOCAL_TIME_ZONE					(-8)

/******************************************************************************/

#endif /* _MSHIELD_H_ */
