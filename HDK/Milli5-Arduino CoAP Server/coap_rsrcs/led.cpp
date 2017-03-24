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
#include <string.h>

#include "mshield.h"
#include "log.h"
#include "bufutil.h"
#include "coapmsg.h"
#include "coappdu.h"
#include "resrc_coap_if.h"
#include "led.h"
#include "arduino_pins.h"

/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/

/*
 * @brief CoAP Resource External LED
 *
 */
error_t crled(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it)
{
    struct optlv *o;

    /* No URI path beyond /led is supported, so reject if present. */
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
     * PUT for writing configuration or enabling sensors
     */
    if (req->code == COAP_REQUEST_PUT) 
    {
        error_t rc = ERR_OK;
        
        /* PUT /led?<on|off|blink_slow|blink_fast> */
        if (!coap_opt_strcmp(o, "on"))
        {
			arduino_put_led_state(LED_ON);
        } 
        else if (!coap_opt_strcmp(o, "off"))
        {
			arduino_put_led_state(LED_OFF);
			
        }
        else if (!coap_opt_strcmp(o, "blink=slow"))
        {
			arduino_put_led_state(LED_BLINK_SLOW);
			
        }
        else if (!coap_opt_strcmp(o, "blink=fast"))
        {
			arduino_put_led_state(LED_BLINK_FAST);
			
        }
        else
        {
            /* Not supported query. */
            rsp->code = COAP_RSP_501_NOT_IMPLEMENTED;
            goto err;
        }
        
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
     * GET for reading sensor or config information
     */
    else if (req->code == COAP_REQUEST_GET)
    {
        uint8_t rc, len = 0;

        /* Config or sensor values. */
        /* GET /led?state */
        if (!coap_opt_strcmp(o, "state"))
        {
            /* Get LED state */
            rc = arduino_get_led_state( rsp->msg, &len );
        }
        else {
            /* Don't support other queries. */
            rsp->code = COAP_RSP_501_NOT_IMPLEMENTED;
            goto err;
        }
        dlog(LOG_DEBUG, "GET (status %d) read %d bytes.", rc, len);
        if (!rc) {
            rsp->plen = len;
            rsp->cf = COAP_CF_CSV;
            rsp->code = COAP_RSP_205_CONTENT;
        } else {
            switch (rc) {
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
    } // if GET
    
    /*
     * DELETE for disabling sensors.
     */
	 
    /* DELETE /led?all */
    else if (req->code == COAP_REQUEST_DELETE) 
    {
        if (!coap_opt_strcmp(o, "all"))
        {
            if (arduino_put_led_state(LED_DISABLE)) 
            {
                rsp->code = COAP_RSP_500_INTERNAL_ERROR;
                goto err;
            }
        } else {
            rsp->code = COAP_RSP_405_METHOD_NOT_ALLOWED;
            goto err;
        }
        rsp->code = COAP_RSP_202_DELETED;
        rsp->plen = 0;
    
    } // if DELETE 
    else 
    {
        /* no other operation is supported */
        rsp->code = COAP_RSP_405_METHOD_NOT_ALLOWED;
        goto err;
        
    } // Unknown operation

done:
    return ERR_OK;

err:
    rsp->plen = 0;

    return ERR_OK;
    
} // crled



// State of external LED
static led_state_t led_state = LED_OFF;
static boolean actuate_flag = LOW;

error_t led_init( uint32_t de, uint32_t count )
{
	uint32_t ix;
	
	// Configure built-in LED pin as output
	pinMode( ONBOARD_LED, OUTPUT );

	// Configure external LED pin for output
	pinMode( LED_PIN, OUTPUT );

	// Blick N times
	for( ix = 0; ix < count; ix++ )
	{
		digitalWrite(LED_PIN,HIGH);
		delay(de);
		digitalWrite(LED_PIN,LOW);
		delay(de);

	} // for
	
	// Set the LED state to off
	led_state = LED_OFF;
  
} // led_init

/*
 * arduino_get_led_state()
 *
 * Get LED status and return to caller in supplied mbuf.
 */
error_t arduino_get_led_state( struct mbuf *m, uint8_t *len )
{
	uint32_t count = 0;
	char p[16];

	// Check the desired state
	switch(led_state)
	{
		case LED_OFF:
		
			// LED is Off
			strcpy( p, "Off" );
			Serial.println("The LED is turned off!");
			break;
			
		case LED_ON:
		
			// LED is On
			strcpy( p, "On" );
			Serial.println("The LED is turned on!");
			break;
			
		case LED_BLINK_SLOW:
		
			// Slow blinking LED
			strcpy( p, "Blinking slow" );
			Serial.println("The LED is blinking slow!");
			break;
			
		case LED_BLINK_FAST:
		
			// Slow blinking LED
			strcpy( p, "Blinking fast" );
			Serial.println("The LED is blinking fast!");
			break;
			
		case LED_DISABLE:

			// Slow blinking LED
			strcpy( p, "Disabled" );
			Serial.println("The LED is disabled!");
			break;
			
		default:
			return ERR_FAIL;

	} // if else
	
	// Assemble response
	rsp_msg( m, len, count, NULL, p );
	
    return ERR_OK;
	
} // arduino_get_led()

/*
 * @brief Set the LED state
 * @params state 
 */
error_t arduino_put_led_state( led_state_t state )
{
	// Save the desired state
	led_state = state;
	actuate_flag = HIGH;
}

/*
 * @brief Turn LED on or off, blink fast/slow or disable the LED
 */
error_t arduino_led_actuate()
{
	// Only do this function if actuate_flag is HIGH
	if (!actuate_flag)
	{
		return ERR_OK;
	}
	actuate_flag = LOW;

	// Check the desired state
	switch(led_state)
	{
		case LED_OFF:
		
			// Turn off LED
			digitalWrite(LED_PIN,LOW);
			break;
			
		case LED_ON:
		
			// Turn on LED
			digitalWrite(LED_PIN,HIGH);
			break;
			
		case LED_BLINK_SLOW:
		
			// Slow blinking LED
			led_init(800,7);
			break;
			
		case LED_BLINK_FAST:
		
			// Fast blinking LED
			led_init(100,50);
			break;
			
		case LED_DISABLE:
			// Turn off LED
			digitalWrite(LED_PIN,LOW);
			break;
			
		default:
			return ERR_FAIL;

	} // if else
	
    error_t ERR_OK;


} // arduino_put_led_state()

/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/
