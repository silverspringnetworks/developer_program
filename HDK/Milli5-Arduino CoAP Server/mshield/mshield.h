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

#include "coap_server.h"
#include "coappdu.h"
#include "coapmsg.h"
#include "coapsensoruri.h"
#include "coap_rbt_msg.h"
#include "temp_sensor.h"
#include "log.h"
#include "arduino_time.h"


//NOTE: CoAP Server Version is defined in coapsensoruri.h


//////////////////////////////////////////////////////////////////////////
//
// List of sensors. These are strings which become part of the CoAP resource URI
// Example: "temp" -> /snsr/arduino/temp
//
// A sensor name has a max of 22 characters. Avoid using characters ,.;:{}-+*&%$#@!?<>|\/[]~`
//
//////////////////////////////////////////////////////////////////////////

// DHT11 temp sensor
#define TEMP_SENSOR           			"temp"

// More examples. You may reuse for your own sensor needs
#define ONE_SENSOR             			"sensor1"
#define TWO_SENSOR       			    "sensor2"


//////////////////////////////////////////////////////////////////////////
//
// One of your sensors may be used for CoAP Observes. At the frequency
// you specify by xxxxxxxxxx a CoAP Notification will be sent to the registered client.
//
//////////////////////////////////////////////////////////////////////////

// Pick one sensor from the sensors defined above to make it an "observable" sensor:
#define OBS_SENSOR_NAME     			TEMP_SENSOR

// Specify the function that reads the sensor and assembles the CoAP Response message
#define OBS_FUNC_PTR        			&arduino_get_temp		// Located in temp_sensor.cpp


//////////////////////////////////////////////////////////////////////////
//
// CoAP Observe Max-Age, see Section 5.10.5 of rfc7252. Default of 90s.
//
//////////////////////////////////////////////////////////////////////////
#define COAP_MSG_MAX_AGE_IN_SECS		90


//////////////////////////////////////////////////////////////////////////
//
// Specify the Serial USB to use for logging and debugging
// Specify the Serial UART to use for HDLC Tx and Rx
//
//////////////////////////////////////////////////////////////////////////

// For Boards that use a Atmel SAMD chip but are not the ADAFRUIT_METRO_M0_EXPRESS board
#ifndef ADAFRUIT_METRO_M0_EXPRESS
// For Boards that use other Atmel SAMD chip family
#if defined(ARDUINO_ARCH_SAMD)
	#define SER_MON_PTR					&SerialUSB
	#define UART_PTR        			&Serial1
#endif

// For Boards that use other Atmel SAM chip families
#if defined(ARDUINO_ARCH_SAM)
	#define SER_MON_PTR				  	&SerialUSB
	#define UART_PTR        			&Serial
#endif
#endif

// For the Adafruit Metro M0 Express (uses the SAMD chip family).
#if defined(ADAFRUIT_METRO_M0_EXPRESS)
	#define SER_MON_PTR				    &Serial
	#define UART_PTR				    &Serial1
#endif


//////////////////////////////////////////////////////////////////////////
//
// Specify the baud rate for the Serial USB
//
//////////////////////////////////////////////////////////////////////////
#define SER_MON_BAUD_RATE     			115200


//////////////////////////////////////////////////////////////////////////
//
// Set the logging level (see log.h)
// Typically you will set this to LOG_INFO or LOG_DEBUG
//
//////////////////////////////////////////////////////////////////////////
#define LOG_LEVEL						LOG_DEBUG


//////////////////////////////////////////////////////////////////////////
//
// Set the UART time-out (the serial link between Arduino and MilliShield)
// Values is in milliseconds
//
//////////////////////////////////////////////////////////////////////////
#define UART_TIMEOUT_IN_MS				2000


//////////////////////////////////////////////////////////////////////////
//
// The largest HDLC payload size. Maximum payload length in the MilliShield is 255
//
//////////////////////////////////////////////////////////////////////////
#define MAX_HDLC_INFO_LEN				(255)

//////////////////////////////////////////////////////////////////////////
//
// Local time zone relative to UTC
// Examples: Pacific: -8, Eastern: -5, London: 0, Paris: +1, Sydney: +10
//
//////////////////////////////////////////////////////////////////////////
#define LOCAL_TIME_ZONE					(-8)


#endif /* _MSHIELD_H_ */
