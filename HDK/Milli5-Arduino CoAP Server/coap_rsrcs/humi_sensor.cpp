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
#include "resrc_coap_if.h"
#include "humi_sensor.h"



/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/


/*
 * crhumidity
 *
 * @brief CoAP Resource humidity sensor
 *
 */
error_t crhumidity(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it)
{
    struct optlv *o;

    /* No URI path beyond /humi is supported, so reject if present. */
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
        
        /* PUT /humi?state=enable */
        if (!coap_opt_strcmp(o, "state=enable"))
        {
			arduino_enab_humi();
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
     * GET /humi
     */
    else if (req->code == COAP_REQUEST_GET)
    {
        uint8_t rc, len = 0;

        /* State or sensor values */
        /* GET /humi?state */
        if (!coap_opt_strcmp(o, "state"))
        {
            /* Get humidity config */
            rc = arduino_get_humi_state( rsp->msg, &len );
        }
        /* GET /humi?sens */
        else if (!coap_opt_strcmp(o, "sens"))
        {
        	/* Get sensor value */
            rc = arduino_get_humi( rsp->msg, &len );
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
    /* DELETE /humi?all */
    else if (req->code == COAP_REQUEST_DELETE) 
    {
        if (!coap_opt_strcmp(o, "all"))
        {
            if (arduino_disab_humi()) 
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
    
} // crhumidity

/*
 * Enable humidity sensor.
 */
error_t arduino_enab_humi()
{
    error_t rc;
    rc = humi_sensor_enable();
    if (!rc) {
        coap_stats.sensors_enabled++;
    }
    return rc;
}


/*
 * Disable humidity sensor.
 */
error_t arduino_disab_humi()
{
    error_t rc;
    rc = humi_sensor_disable();
    if (!rc) {
        coap_stats.sensors_disabled++;
    }
    return rc;
}

extern DHT_Unified dht;

error_t arduino_humi_sensor_init()
{
	// Enable temp sensor
	arduino_enab_humi();
	
	sensor_t sensor;
	dht.humidity().getSensor(&sensor);
	Serial.println("------------------------------------");
	Serial.println("Humidity");
	Serial.print  ("Sensor:       "); Serial.println(sensor.name);
	Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
	Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
	Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
	Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
	Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
	Serial.println("------------------------------------");
  
} // humi_sensor_init

// Humidity sensor configuration
humi_sensor_cfg_t humi_sensor_cfg;

/*
 * @brief  Get humidity sensor state
 * @return error_t
 */
error_t arduino_get_humi_state( struct mbuf *m, uint8_t *len )
{
    error_t rc = ERR_OK;
	char buf[32];
	const uint32_t count = 0;
	
	switch(humi_sensor_cfg.state)
	{
		case LOW:
			strcpy(buf,"Disabled");
			break;
			
		case HIGH:
			strcpy(buf,"Enabled");
			break;
			
		default:
			return ERR_FAIL;
			
	} // switch
	rc = rsp_msg( m, len, count, NULL, buf );
	return rc;
	
} // arduino_get_humi_state()

/*
 * arduino_get_humi()
 *
 * Get temperature data and return to caller in supplied mbuf.
 */
error_t arduino_get_humi( struct mbuf *m, uint8_t *len )
{
	const uint32_t count = 1;
	float reading = 0.0;
    error_t rc;

	if ( humi_sensor_cfg.state == LOW )
	{
		return ERR_OP_NOT_SUPP;
		
	} // if

    /* Read humidity */
    rc = humi_sensor_read( &reading );
	
	// Assemble response
	rsp_msg( m, len, count, &reading, "%" );

    return rc;
}


/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/

error_t humi_sensor_read( float * p )
{
    error_t rc = ERR_OK;
	float hum = 0.0;

	// Get temperature event and print its value.
	sensors_event_t event;  
	dht.temperature().getEvent(&event);
	// Get humidity event and print its value.
	dht.humidity().getEvent(&event);
	if (isnan(event.relative_humidity))
	{
		Serial.println("Error reading humidity!");
		rc = ERR_FAIL;
	}
	else 
	{
		hum = event.relative_humidity;
		*p = hum;
		rc = ERR_OK;
	}

    return rc;
}

error_t humi_sensor_enable(void)
{
	humi_sensor_cfg.state = HIGH;
    return ERR_OK;
}

error_t humi_sensor_disable(void)
{
	humi_sensor_cfg.state = LOW;
    return ERR_OK;
}
