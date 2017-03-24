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
#include "exp_coap.h"
#include "coapmsg.h"
#include "coappdu.h"
#include "arduino_pins.h"
#include "resrc_coap_if.h"
#include "light_sensor.h"


/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/

// Light sensor Config
light_sensor_cfg_t light_sensor_cfg;

/*
 * @brief CoAP Resource light sensor
 *
 */
error_t crlight(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it)
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
        
        /* PUT /light?state=enable */
        if (!coap_opt_strcmp(o, "state=enable"))
        {
			arduino_enab_light();
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
     * GET /light
     */
    else if (req->code == COAP_REQUEST_GET)
    {
        uint8_t rc, len = 0;

        /* State or sensor values */
        /* GET /light?state */
        if (!coap_opt_strcmp(o, "state"))
        {
            /* Get light sensor config */
            rc = arduino_get_light_state( rsp->msg, &len );
        }
        /* GET /light?sens */
        else if (!coap_opt_strcmp(o, "sens"))
        {
        	/* Get sensor value */
            rc = arduino_get_light( rsp->msg, &len );
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
    /* DELETE /light?all */
    else if (req->code == COAP_REQUEST_DELETE) 
    {
        if (!coap_opt_strcmp(o, "all"))
        {
            if (arduino_disab_light()) 
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
    
} // crlight

/*
 * Enable temp sensor.
 */
error_t arduino_enab_light()
{
    error_t rc;
    rc = light_sensor_enable();
    if (!rc) {
        coap_stats.sensors_enabled++;
    }
    return rc;
}


/*
 * Disable light sensor.
 */
error_t arduino_disab_light()
{
    error_t rc;
    rc = light_sensor_disable();
    if (!rc) {
        coap_stats.sensors_disabled++;
    }
    return rc;
}

/*
 * @brief Init light sensor
 * @return error_t
 *
 */
error_t arduino_light_sensor_init()
{
	// Enable
	arduino_enab_light();
	
} // arduino_init_light

/**
 * @brief Get light sensor state
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_light_state(struct mbuf *m, uint8_t *len)
{
    error_t rc = ERR_OK;
	char buf[32];
	const uint32_t count = 0;
	switch(light_sensor_cfg.state)
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
}

/**
 * @brief Set light sensor config
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_set_light_cfg(struct mbuf *m, uint8_t *len)
{
    return ERR_OK;
}

/**
 * @brief Get light sensor config
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_light_cfg(struct mbuf *m, uint8_t *len)
{
    return ERR_OK;
}


/*
 * @brief
 * @return error_t
 * 
 */
error_t arduino_get_light( struct mbuf *m, uint8_t *len )
{
	const uint32_t count = 1;
	float reading;
    error_t rc = ERR_OK;

	if ( light_sensor_cfg.state == LOW )
	{
		return ERR_OP_NOT_SUPP;
		
	} // if
	
	// Read sensor
	rc = light_sensor_read(&reading);

	// Assemble response
	rc = rsp_msg( m, len, count, &reading, "Lux" );
	
    return rc;
}


/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/

/*
 * @brief
 * @return error_t
 */
error_t light_sensor_enable(void)
{
	light_sensor_cfg.state = HIGH;
    return ERR_OK;
}

/*
 * @brief
 * @return error_t
 */
error_t light_sensor_disable(void)
{
	light_sensor_cfg.state = LOW;
    return ERR_OK;
}

/**
 * @brief Read sensor 
 *
 * @return error_t
 */
error_t light_sensor_read( float * p )
{
    error_t rc = ERR_OK;
	float rawRange = 1024;	// 3.3v
	float logRange = 5.0;	// 3.3v = 10^5 lux
	float rawValue = (float) analogRead(LIGHT_PIN);
	float logLux = rawValue * logRange / rawRange;
	*p = pow(10,logLux);
    return rc;
}

