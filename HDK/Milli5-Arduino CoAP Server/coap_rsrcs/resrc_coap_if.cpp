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
#include "mshield.h"
#include "log.h"
#include "hbuf.h"
#include "exp_coap.h"
#include "coapsensoruri.h"
#include "coappdu.h"
#include "resrc_coap_if.h"
#include "temp_sensor.h"
#include "humi_sensor.h"
#include "xyz_sensor.h"
#include "light_sensor.h"
#include "led.h"
#include "arduino_time.h"


/*! @brief
 * Access is serialised via events, only one client too. Only need to handle
 * one observe per sensor. We have 3 sensors.
 * To keep memory usage down, only support 1 callback function.
 */
#define NUM_ARDUINO_RESOURCES   (crdt_max_sens - crdt_min_sens + 1)
static void *coap_hdl[NUM_ARDUINO_RESOURCES] = { NULL };
static uint8_t hdr_rsv_bytes;
struct coap_stats coap_stats;

/* 
 * TODO: Check ct, and see if format above can/should be
 * optimised. See https://tools.ietf.org/html/rfc6690.
 * Use #defines L_URI*
 */

/*
 * See https://zoo.silverspringnet.com/x/JOLBAw for documented URIs supported
 * below.
 *
 */
 



/*
 * @brief crarduino
 * 
 *
 */
error_t crarduino( struct coap_msg_ctx *req, struct coap_msg_ctx *rsp )
{
    struct optlv *o;
    void *it = NULL;

    /* 
     * No URI path beyond /arduino, except /humi and /temp is supported, so
     * reject if present. 
     */
    copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it);
    if ((o = copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it))) 
	{
        if (!coap_opt_strcmp( o, "humi" ))
		{
            return crhumidity( req, rsp, it );
        }
		
		if (!coap_opt_strcmp( o, "temp" ))
		{
            return crtemperature( req, rsp, it );
        }

		if (!coap_opt_strcmp( o, "xyz" ))
		{
            return crxyz( req, rsp, it );
        }

		if (!coap_opt_strcmp( o, "light" ))
		{
            return crlight( req, rsp, it );
        }
		
		if (!coap_opt_strcmp( o, "led" ))
		{
            return crled( req, rsp, it );
        }

		if (!coap_opt_strcmp( o, "time" ))
		{
            return crtime( req, rsp, it );
        }

        rsp->code = COAP_RSP_404_NOT_FOUND;
		
    } // if            

    rsp->plen = 0;

    return ERR_OK;
	
} // crarduino


// Assemble a CoAP response message; {Timestamp,Value(s),Unit}
error_t rsp_msg( struct mbuf * m, uint8_t *len, uint32_t count, float * reading, char * unit )
{
    uint8_t 	l;
	char 		rsp_buf[256];
	char		reading_buf[128];
	char		unit_buf[32];
	char * 		p;
	time_t     	epoch;
	uint32_t	ix;
	
	// Create string containing the UNIX epoch
	epoch = get_rtc_epoch();
	sprintf( rsp_buf, "%d", epoch );
	
	// Check if we have a sensor reading
	if (reading)
	{
		// Get each value
		for( ix = 0; ix < count; ix++ )
		{
			// Create string containing reading
			sprintf( reading_buf, ",%.2f", *reading++ );

			// Concatenate the reading
			strcat( rsp_buf, reading_buf );
			
		} // for
	} // if

	// Check if we have a unit
	if (unit)
	{
		// Concatenate the unit
		sprintf( unit_buf, ",%s", unit );
		strcat( rsp_buf, unit_buf );
		
	} // if

	// Print message
	Serial.println(rsp_buf);
	
	// Get length
	l = strlen(rsp_buf);
	
	// Allocate memory
	p = (char*) m_append(m,l);
    if (!p) 
    {
        return ERR_NO_MEM;
    }

	// Store output
	strcpy(p,rsp_buf);
	*len = l;
	
    return ERR_OK;
	
} // rsp_msg()


/*
 * Init sensors and other resources
 */
error_t arduino_init_resources()
{
    error_t rc = ERR_OK;
	
	// Init and blink LED
	// Delays enough for Tera Term to re-connect to the Native port
	led_init( 200, 10 );

	// Init RTC time object
	rtc_time_init();	
	
	// Init temp sensor
	arduino_temp_sensor_init();
	
	// Init humidity sensor
	arduino_humi_sensor_init();
	
	// Init XYZ sensor
	arduino_xyz_sensor_init();
	
	// Init light sensor
	arduino_light_sensor_init();
	
    return rc;
}

/*
 * Enable sensors. Typically only called for initial device activation.
 */
error_t arduino_enab_sensors()
{
    error_t rc;
    rc = arduino_enab_temp();
    return rc;
}


/*
 * Disable sensors. Would be called after validating mNIC, before shipping to
 * customer.
 */
error_t arduino_disab_sensors()
{
    error_t rc;
    rc = arduino_disab_temp();
	return rc;
}
