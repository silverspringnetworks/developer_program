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


/*
 * crtime
 *
 * @brief CoAP Resource Arduino time
 *
 */
error_t crtime(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it)
{
    struct optlv *o;

    /* No URI path beyond /time is supported, so reject if present. */
    o = copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it);
    if (o)
    {
        rsp->code = COAP_RSP_404_NOT_FOUND;
        goto err;
    }            

    /* All methods require a query, so return an error if missing. */
    if (!(o = copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_QUERY, NULL))) 
    {
        rsp->code = COAP_RSP_405_METHOD_NOT_ALLOWED;
        goto err;
    }
    
    /*
     * PUT for setting Arduino time
     */
    if (req->code == COAP_REQUEST_PUT) 
    {
        error_t rc = ERR_OK;
		char * p;
		time_t epoch;
        
        /* PUT /time?1488953919 */
		p = (char*) (o->ov);

		// UTC time
		epoch = (time_t) atoi(p);
		print("Epoch: "); printnum(epoch); println("");
		
		// Convert to the local time zone
		epoch += seconds_relative_utc;
		rtc.setEpoch(epoch);
		
		// Print time/date
		print_current_date();

		// Check return code
        if (!rc)
        {
            rsp->code = COAP_RSP_204_CHANGED;
            rsp->plen = 0;
        }
        else
        {
            switch (rc)
            {
            case ERR_BAD_DATA:
            case ERR_INVAL:
                rsp->code = COAP_RSP_406_NOT_ACCEPTABLE;
                break;
            default:
                rsp->code = COAP_RSP_500_INTERNAL_ERROR;
                break;
            }
            goto err;
        }
    } // if PUT
    
    /*
     * GET /time
     */
    else if (req->code == COAP_REQUEST_GET)
    {
        uint8_t rc, len = 0;
		uint32_t count = 0;
		
        /* GET /time?whatever */
		rc = rsp_msg( rsp->msg, &len, count, NULL, NULL );

        dlog(LOG_DEBUG, "GET (status %d) read %d bytes.", rc, len);
        if (!rc)
		{
            rsp->plen = len;
            rsp->cf = COAP_CF_CSV;
            rsp->code = COAP_RSP_205_CONTENT;
        }
		else
		{
            switch (rc)
			{
            case ERR_BAD_DATA:
            case ERR_INVAL:
                rsp->code = COAP_RSP_406_NOT_ACCEPTABLE;
                break;
            default:
                rsp->code = COAP_RSP_500_INTERNAL_ERROR;
                break;
            }
            goto err;
        }
    }
    else 
    {
        /* no other operation is supported */
        rsp->code = COAP_RSP_405_METHOD_NOT_ALLOWED;
        goto err;
    }

done:
    return ERR_OK;

err:
    rsp->plen = 0;
    return ERR_OK;
} // crtime


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
	sprintf( buffer, "Date: %02d:%02d:%d [mon:day:year]", c, b, a );
	println(buffer);
	
} // print_current_date
