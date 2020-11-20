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

#include "log.h"
#include "bufutil.h"
#include "coap_rsp_msg.h"
#include "coapmsg.h"
#include "coappdu.h"
#include "arduino_time.h"


// Atmel SAMDxxx chip family RTC support
#if defined(ARDUINO_ARCH_SAMD)
#include <RTCZero.h>
RTCZero rtc;
#endif

// STM32 L4xx chip family RTC support
#if defined(STM32L4xx)
#include <STM32RTC.h>
STM32RTC& rtc = STM32RTC::getInstance();
#endif


// Time relative UTC
static int32_t seconds_relative_utc = 0;

/*
 * @brief Initialize the RTC. Note this depends on the ARM MCU and associated RTC code. 
 *
 */
#if defined(ARDUINO_ARCH_SAMD)
error_cs_t init_rtc( int32_t zone )
{
	// Initialize RTC time
	rtc.begin();
	rtc.setTime(0,0,0);
	rtc.setDate(0,0,0);
	
	// Set the timezone
	set_time_zone(zone);
	
} // init_rtc()
#endif

#if defined(STM32L4xx)
error_cs_t init_rtc( int32_t zone )
{
	// Set the timezone
	set_time_zone(zone);
	
} // init_rtc()
#endif


/*
 * @brief Get the current RTC epoch
 * 
 */
uint32_t get_rtc_epoch()
{
	// Get current epoch
	uint32_t epoch;
	epoch = rtc.getEpoch();
	return epoch;

} // get_rtc_epoch()


/**
 * @brief Set RTC epoch
 *
 */
void set_rtc_epoch(uint32_t epoch)
{
	rtc.setEpoch(epoch);
	
} // set_rtc_epoch()



/**
* @brief
* Prints the current time
* The time is set to 0 during boot
*
*/
void print_current_time(void)
{
	uint32_t a,b,c;
	char buffer[PRINTF_LEN];

	// Print time
	a = rtc.getSeconds();
	b = rtc.getMinutes();
	c = rtc.getHours();
	sprintf( buffer, "Time: %02d:%02d:%02d [hr:min:sec]", c, b, a );
	println(buffer);
	
} // print_current_time


/**
* @brief
* Prints the current time specifically for logging
* The time is set to 0 during boot
*
*/
void print_log_time(void)
{
	uint32_t a,b,c;
	char buffer[PRINTF_LEN];

	// Print time
	a = rtc.getSeconds();
	b = rtc.getMinutes();
	c = rtc.getHours();
	sprintf( buffer, "Time: %02d:%02d:%02d: ", c, b, a );
	print(buffer);
	
} // print_current_time


/**
* @brief
* Prints the current date
* The date is set to 0 during boot
*
*/
void print_current_date(void)
{
	uint32_t a,b,c;
	char buffer[PRINTF_LEN];

	// Print time
	a = rtc.getYear();
	b = rtc.getDay();
	c = rtc.getMonth();
	sprintf( buffer, "Date: %02d:%02d:%02d [mon:day:year]", c, b, a );
	println(buffer);
	
} // print_current_date


/**
 * @brief Set time zone
 *
 */
error_cs_t set_time_zone( int32_t zone )
{
	seconds_relative_utc = zone*60*60;
	
} // set_time_zone


/*
 * @brief Get the current epoch. Stub for future portability.
 * 
 */
uint32_t get_epoch()
{
	// Get current epoch
	uint32_t epoch;
	epoch = get_rtc_epoch();
	return epoch;

} // get_epoch()
