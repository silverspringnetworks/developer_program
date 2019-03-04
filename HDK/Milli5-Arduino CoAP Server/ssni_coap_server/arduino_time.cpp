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

#include "log.h"
#include "bufutil.h"
#include "coap_rsp_msg.h"
#include "coapmsg.h"
#include "coappdu.h"
#include "arduino_time.h"


#if defined(ARDUINO_ARCH_SAMD)
RTCZero rtc;
#endif

#if defined(ARDUINO_ARCH_SAM)
RTCDue rtc(XTAL);
#endif

// Time relative UTC
static int32_t seconds_relative_utc = 0;


/**
 * @brief Set time zone
 *
 */
error_t set_time_zone( int32_t zone )
{
	seconds_relative_utc = zone*60*60;
	
} // set_time_zone


/*
 *
 * @brief Init the RTC time
 *
 */
error_t rtc_time_init( int32_t zone )
{
	// Init RTC time
	rtc.begin();
	rtc.setTime(0,0,0);
	rtc.setDate(0,0,0);
	
	// Set the timezone
	set_time_zone(zone);
	
} // rtc_time_init()


/*
 * @brief Get the RTC time in local time
 *
 * 
 */
time_t get_rtc_epoch()
{
	// Convert to UTC
	time_t epoch;
	epoch = rtc.getEpoch();
	return epoch;

} // get_rtc_time()


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
