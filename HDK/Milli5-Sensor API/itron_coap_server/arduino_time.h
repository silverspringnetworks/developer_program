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

#ifndef ARDUINO_TIME_H_
#define ARDUINO_TIME_H_

#include <arduino.h>
#include "errors.h"

#if defined(ARDUINO_ARCH_SAMD)
#include <RTCZero.h>
extern RTCZero rtc;
#endif

#if defined(ARDUINO_ARCH_SAM)
#include <RTCDue.h>
extern RTCDue rtc;
#define getEpoch unixtime
#define setEpoch setClock
#endif


/**
 * @brief Set time zone
 *
 */
error_t set_time_zone( int32_t zone );

/**
 * @brief
 *
 */
error_t rtc_time_init( int32_t zone );

/**
 *
 * @brief Get RTC epoch
 *
 */
time_t get_rtc_epoch();

/**
* @brief
* Prints the current time
* The time is set to 0 during boot
*
*/
void print_current_time(void);

/**
* @brief
* Prints the current time specifically for logging
* The time is set to 0 during boot
*
*/
void print_log_time(void);

/**
* @brief
* Prints the current date
* The date is set to 0 during boot
*
*/
void print_current_date(void);

#endif /* ARDUINO_TIME_H_ */
