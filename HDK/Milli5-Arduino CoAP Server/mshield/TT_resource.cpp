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
#include "exp_coap.h"
#include "coap_rsp_msg.h"
#include "coappdu.h"
#include "coapmsg.h"
#include "arduino_pins.h"
#include "TT_resource.h"


/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/

// TT Config
TT_cfg_t TT_cfg;

/**
 * crtemperature
 *
 * @brief CoAP Resource "describe your sensor here"
 *
 */
error_t crmysensor(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it)
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
     * PUT for writing configuration or enabling the sensor
     */
    if (req->code == COAP_REQUEST_PUT) 
    {
        error_t rc = ERR_OK;

        /* PUT /uri?cfg=<A|B> */
        if (!coap_opt_strcmp(o, "cfg=A"))
        {
			//arduino_put_temp_cfg(CELSIUS_SCALE);
        } 
        else if (!coap_opt_strcmp(o, "cfg=B"))
        {
			//arduino_put_temp_cfg(FAHRENHEIT_SCALE);
			
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
            //rc = arduino_get_temp_cfg( rsp->msg, &len );
        }
        /* GET /temp?sens */
        else if (!coap_opt_strcmp(o, "sens"))
        {
			if ((o = copt_get_next_opt_type((sl_co*)&(req->oh), COAP_OPTION_OBSERVE, NULL))) 
			{
				uint32_t obsval = co_uint32_n2h(o);
				switch(obsval)
				{
					//case COAP_OBS_REG:
						//rc = coap_obs_reg();
						break;
					
					//case COAP_OBS_DEREG:
						//rc = coap_obs_dereg();
						break;
					
					default:
						rc = ERR_INVAL;

				} // switch
			}
			else
			{
				/* Get sensor value */
				//rc = arduino_get_temp( rsp->msg, &len );
				
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
            //if (arduino_disab_temp()) 
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
    
} // crmysensor

/**
 * @brief Enable temp sensor.
 */
error_t arduino_enab_TT()
{
    error_t rc;
    rc = TT_enable();
    if (!rc) {
        coap_stats.sensors_enabled++;
    }
    return rc;
}


/**
 * @brief Disable TT sensor.
 */
error_t arduino_disab_TT()
{
    error_t rc;
    rc = TT_disable();
    if (!rc) {
        coap_stats.sensors_disabled++;
    }
    return rc;
}

/**
 * @brief
 *
 */
error_t arduino_init_TT()
{
} // arduino_init_TT

/**
 * @brief Set TT state
 * @param[in] state Desired state
 * @return error_t
 */
error_t arduino_set_TT_state( uint32_t state )
{
    return ERR_OK;
}

/**
 * @brief Get TT state
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_TT_state(struct mbuf *m, uint8_t *len)
{
	error_t rc = ERR_OK;
	char buf[32];
	const uint32_t count = 0;
	switch(TT_cfg.state)
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
 * @brief Set TT config
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_set_TT_cfg(struct mbuf *m, uint8_t *len)
{
    return ERR_OK;
}

/**
 * @brief Get TT config
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_TT_cfg(struct mbuf *m, uint8_t *len)
{
    return ERR_OK;
}


/**
 * @brief A function that reads a sensor and assembles a return message
 * @param[out] m Pointer to return message
 * @param[out] len Length of return message
 * @return error_t
 * 
 */
error_t arduino_get_TT( struct mbuf * m, uint8_t *len )
{
    error_t rc = ERR_OK;
	const uint32_t count = 1; 	// Number of readings; if more than 1, supply 
								// pointer to an array in the call to rsp_msg() below
	float reading;				// The sensor reading
								// If the sensor returns more than one value,
								// declare this as an array of floats
	char unit[2] = "F";			// The measurement unit e.g. "F" (Fahrenheit)
								// NOTE: you need to allocate space for string termination \0 character
								// In this case that means you need two characters;
								// one for the unit F and one for \0
	// Read the sensor
	// The return value is stored in the variable 'reading'
	rc = TT_read(&reading);
	
	// Assemble response based on the reading(s) and the Unit of Measure
	// The function rsp_msg() is declared in coap_rsp_msg.h
	rc = rsp_msg( m, len, count, &reading, unit );
    return rc;
}


/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/

/*
 * @brief Enable sensor
 * @return error_t
 */
error_t TT_enable(void)
{
	TT_cfg.state = HIGH;
    return ERR_OK;
}

/*
 * @brief Disable sensor
 * @return error_t
 */
error_t TT_disable(void)
{
	TT_cfg.state = LOW;
    return ERR_OK;
}

/**
 * @brief Read sensor 
 *
 * @return error_t
 */
error_t TT_read( float * p )
{
	// Add code to read the sensor here and assign a value to where p is pointing
	// *p = 
    error_t rc = ERR_OK;
    return rc;
}

