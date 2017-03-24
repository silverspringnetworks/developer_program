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

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "mshield.h"
#include "log.h"
#include "bufutil.h"
#include "exp_coap.h"
#include "coapmsg.h"
#include "coappdu.h"
#include "coapobserve.h"
#include "coapsensorobs.h"
#include "resrc_coap_if.h"
#include "temp_sensor.h"
#include "arduino_pins.h"


/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/

/*
 * crtemperature
 *
 * @brief CoAP Resource temperature sensor
 *
 */
error_t crtemperature(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it)
{
    struct optlv *o;

    /* No URI path beyond /temp is supported, so reject if present. */
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

        /* PUT /temp?cfg=<C|F> */
        if (!coap_opt_strcmp(o, "cfg=C"))
        {
			arduino_put_temp_cfg(CELSIUS_SCALE);
        } 
        else if (!coap_opt_strcmp(o, "cfg=F"))
        {
			arduino_put_temp_cfg(FAHRENHEIT_SCALE);
			
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
        /* GET /temp?cfg */
        if (!coap_opt_strcmp(o, "cfg"))
        {
            /* get temperature config */
            rc = arduino_get_temp_cfg( rsp->msg, &len );
        }
        /* GET /temp?sens */
        else if (!coap_opt_strcmp(o, "sens"))
        {
			if ((o = copt_get_next_opt_type((sl_co*)&(req->oh), COAP_OPTION_OBSERVE, NULL))) 
			{
				uint32_t obsval = co_uint32_n2h(o);
				switch(obsval)
				{
					case COAP_OBS_REG:
						rc = coap_obs_reg();
						break;
					
					case COAP_OBS_DEREG:
						rc = coap_obs_dereg();
						break;
					
					default:
						rc = ERR_INVAL;

				} // switch
			}
			else
			{
				/* Get sensor value */
				rc = arduino_get_temp( rsp->msg, &len );
				
			} // if-else
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
    /* DELETE /temp?all */
    else if (req->code == COAP_REQUEST_DELETE) 
    {
        if (!coap_opt_strcmp(o, "all"))
        {
            if (arduino_disab_temp()) 
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
    
} // crtemperature


// TODO: What is this used for
temp_ctx_t temp_ctx;

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview
#define DHT_TYPE           DHT11     // DHT 11 
DHT_Unified dht( A4, DHT_TYPE );

error_t arduino_temp_sensor_init()
{
	// Initialize temperature/humidity sensor
	dht.begin();

	// Enable temp sensor
	arduino_enab_temp();
	
	Serial.println("DHTxx Unified Sensor Example");
	// Print temperature sensor details.
	sensor_t sensor;
	dht.temperature().getSensor(&sensor);
	Serial.println("------------------------------------");
	Serial.println("Temperature");
	Serial.print  ("Sensor:       "); Serial.println(sensor.name);
	Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
	Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
	Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
	Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
	Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
	Serial.println("------------------------------------");
  
} // temp_sensor_init



/*
 * Enable temp sensor.
 */
error_t arduino_enab_temp()
{
    error_t rc;
    rc = temp_sensor_enable();
    if (!rc) {
        coap_stats.sensors_enabled++;
    }
    return rc;
}


/*
 * Disable temp sensor.
 */
error_t arduino_disab_temp()
{
    error_t rc;
    rc = temp_sensor_disable();
    if (!rc) {
        coap_stats.sensors_disabled++;
    }
    return rc;
}

// Scale of temperature measurement
static temp_scale_t temp_scale = FAHRENHEIT_SCALE;

/*
 * arduino_get_temp_scale()
 *
 * Get temperature scale; Celsius or Fahrenheit
 */
char arduino_get_temp_scale()
{
	if ( temp_scale == CELSIUS_SCALE )
	{
		// Celsius
		return 'C';
		
	} // if

	// Fahrenheit
	return 'F';
	
} // arduino_get_temp_scale

/*
 * arduino_get_temp()
 *
 * Get temperature data and return to caller in supplied mbuf.
 */
error_t arduino_get_temp( struct mbuf *m, uint8_t *len )
{
	const uint32_t count = 1;
	float reading = 0.0;
	char unit[2] = "F"; // Default scale Fahrenheit, NULL terminated string
    error_t rc;

	// Check if disabled
	if ( temp_ctx.state == tsat_disabled )
	{
		return ERR_OP_NOT_SUPP;
		
	} // if

	/* Read temp, already in network order. */
    rc = temp_sensor_read(&reading);

	// Check scale
	if ( temp_scale == CELSIUS_SCALE )
	{
		// Change return unit to Celsius
		strcpy( unit, "C" );
		
	} // if

	// Assemble response
	rc = rsp_msg( m, len, count, &reading, unit );

	// Return code
    return rc;
}


/* @brief
 * arduino_put_temp_cfg()
 *
 * Set temp config
 */
error_t arduino_put_temp_cfg( temp_scale_t scale )
{
	switch(scale)
	{
	case CELSIUS_SCALE:
		temp_scale = CELSIUS_SCALE;
       	Serial.println("Celsius scale set!");
		break;

	case FAHRENHEIT_SCALE:
		temp_scale = FAHRENHEIT_SCALE;
       	Serial.println("Fahrenheit scale set!");
		break;

	default:
		return ERR_INVAL;
		
	} // switch

	// Enable
	arduino_enab_temp();
	return ERR_OK;

} // arduino_put_temp_cfg()

/*
 * arduino_get_temp_cfg()
 *
 * Get temperature sensor scale
 */
error_t arduino_get_temp_cfg( struct mbuf *m, uint8_t *len )
{
    error_t rc;
	uint32_t count = 0;
	char unit[2] = "F"; // Default scale Fahrenheit, NULL terminated string

	// Check scale
	if ( temp_scale == CELSIUS_SCALE )
	{
		// Change return unit to Celsius
		strcpy( unit, "C" );
		
	} // if

	// Assemble response
	rsp_msg( m, len, count, NULL, unit );

	return ERR_OK;
}


/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/

error_t temp_sensor_read(float * p)
{
    error_t rc = ERR_OK;
	float re = INVALID_TEMP;

    if (temp_ctx.state == tsat_disabled)
    {
		Serial.println("Temp sensor disabled!");
        return ERR_OP_NOT_SUPP;
    }

	// Get temperature event and print its value.
	sensors_event_t event;  
	dht.temperature().getEvent(&event);

	// Check for NaN
	if (isnan(event.temperature)) 
	{
		Serial.println("Error reading temperature!");
		rc = ERR_FAIL;
	}
	else 
	{
		re = event.temperature;
		if ( FAHRENHEIT_SCALE == temp_scale )
		{
			// Convert from Celsius to Fahrenheit
			re *= 1.8;
			re += 32;
		}
		
		rc = ERR_OK;
	}
	
	// Assign output
	*p = re;
    return rc;
}

error_t temp_sensor_enable(void)
{
    temp_ctx.cfg.enable = 1;
    temp_ctx.state = tsat_cleared;
    return ERR_OK;
}

error_t temp_sensor_disable(void)
{
    temp_ctx.cfg.enable = 0;
    temp_ctx.state = tsat_disabled;
    return ERR_OK;
}