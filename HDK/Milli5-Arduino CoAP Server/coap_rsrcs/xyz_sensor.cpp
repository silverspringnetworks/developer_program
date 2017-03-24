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
#include "exp_coap.h"
#include "coapmsg.h"
#include "coappdu.h"
#include "resrc_coap_if.h"
#include "xyz_sensor.h"


/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/

// XYZ Config
xyz_sensor_cfg_t xyz_sensor_cfg;

/*
 * crxyz
 *
 * @brief CoAP Resource XYZ sensor
 *
 */
error_t crxyz( struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it )
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
     * PUT /xyz
     */
    if (req->code == COAP_REQUEST_PUT) 
    {
        error_t rc = ERR_OK;
        
        /* PUT /xyz?state=enable */
        if (!coap_opt_strcmp(o, "state=enable"))
        {
			arduino_enab_xyz();
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
     * GET /xyz
     */
    else if (req->code == COAP_REQUEST_GET)
    {
        uint8_t rc, len = 0;

        /* State or sensor values */
        /* GET /xyz?state */
        if (!coap_opt_strcmp(o, "state"))
        {
            /* Get XYZ config */
            rc = arduino_get_xyz_state( rsp->msg, &len );
        }
        /* GET /xyz?sens */
        else if (!coap_opt_strcmp(o, "sens"))
        {
        	/* Get sensor value */
            rc = arduino_get_xyz( rsp->msg, &len );
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
    /* DELETE /xyz?all */
    else if (req->code == COAP_REQUEST_DELETE) 
    {
        if (!coap_opt_strcmp(o, "all"))
        {
            if (arduino_disab_xyz()) 
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
    
} // crxyz


/*
An Arduino code example for interfacing with the HMC5883

by: Jordan McConnell
 SparkFun Electronics
 created on: 6/30/11
 license: OSHW 1.0, http://freedomdefined.org/OSHW

Analog input 4 I2C SDA
Analog input 5 I2C SCL
*/
#include <Wire.h> //I2C Arduino Library

#define HMC5883_ADDRESS 0x1E //0011110b, I2C 7bit address of HMC5883
#define NUM_AXIS		3
#define REGS_PER_AXIS	2
#define NUM_REGS		(REGS_PER_AXIS*NUM_AXIS)
#define XYZ_MODE_REG  0x2 // select mode register
#define XYZ_MODE      0x0 // continuous measurement mode


error_t arduino_xyz_sensor_init()
{
	// Enable
	arduino_enab_xyz();
	
	// Initialize I2C communications
	Wire.begin();

	// Put the HMC5883 IC into the correct operating mode
	Wire.beginTransmission(HMC5883_ADDRESS); // open communication with HMC5883
	Wire.write(XYZ_MODE_REG); 
	Wire.write(XYZ_MODE); 
	Wire.endTransmission();

} // xyz_sensor_init

/*
 * arduino_get_xyz()
 *
 * 
 */
error_t arduino_get_xyz( struct mbuf *m, uint8_t *len )
{
    error_t rc = ERR_OK;
	int x,y,z;
	float vals[3];
	const uint32_t count = 3;

	if ( xyz_sensor_cfg.state == LOW )
	{
		return ERR_OP_NOT_SUPP;
		
	} // if
	
	// Read the XYZ sensor
	rc = xyz_sensor_read( &x, &y, &z );
	if (!rc)
	{
		// Assemble response
		vals[0] = (float) x;
		vals[1] = (float) y;
		vals[2] = (float) z;
		rc = rsp_msg( m, len, count, vals, "Gauss" );
		
	} // rc
    return rc;
	
} // arduino_get_xyz()

/*
 * Enable temp sensor.
 */
error_t arduino_enab_xyz()
{
    error_t rc;
    rc = xyz_sensor_enable();
    if (!rc) {
        coap_stats.sensors_enabled++;
    }
    return rc;
}


/*
 * Disable XYZ sensor.
 */
error_t arduino_disab_xyz()
{
    error_t rc;
    rc = xyz_sensor_disable();
    if (!rc) {
        coap_stats.sensors_disabled++;
    }
    return rc;
}

/*
 * @brief
 * @return error_t
 */
error_t arduino_get_xyz_state(struct mbuf *m, uint8_t *len)
{
    error_t rc = ERR_OK;
	char buf[32];
	const uint32_t count = 0;
	switch(xyz_sensor_cfg.state)
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

} // arduino_get_xyz_state

/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/


error_t xyz_sensor_enable(void)
{
	xyz_sensor_cfg.state = HIGH;
    return ERR_OK;
}

error_t xyz_sensor_disable(void)
{
	xyz_sensor_cfg.state = LOW;
    return ERR_OK;
}


/*
 * xyz_sensor_read()
 *
 * 
 */
error_t xyz_sensor_read( int * pX, int * pY, int * pZ )
{
    error_t rc = ERR_OK;

	// Triple axis data
	int x,y,z; 

	// Tell the HMC5883 where to begin reading data
	Wire.beginTransmission(HMC5883_ADDRESS);
	// Select register 3, X MSB register
	Wire.write(0x03); 
	Wire.endTransmission();

	// Read data from each axis, 2 registers per axis
	Wire.requestFrom( HMC5883_ADDRESS, NUM_REGS );
	if( NUM_REGS <= Wire.available())
	{
		x  = Wire.read() << 8; 	// X msb
		x |= Wire.read(); 		// X lsb
		z  = Wire.read() << 8; 	// Z msb
		z |= Wire.read(); 		// Z lsb
		y  = Wire.read() << 8; 	// Y msb
		y |= Wire.read(); 		// Y lsb

	}
	else
	{
		return ERR_BAD_DATA;
		
	} // if-else

	// Return values
	*pX = x;
	*pY = y;
	*pZ = z;

	// Print out values of each axis
	Serial.print("x: ");
	Serial.print(x);
	Serial.print("  y: ");
	Serial.print(y);
	Serial.print("  z: ");
	Serial.println(z);

    return rc;
	
} // xyz_sensor_read()

