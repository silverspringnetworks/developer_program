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

// See Adafruit guide for details on DHT11 sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

#include <string.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include "log.h"
#include "cbor.h"
#include "exp_coap.h"
#include "coap_rsp_msg.h"
#include "coappdu.h"
#include "coapobserve.h"
#include "coapsensorobs.h"
#include "temp_sensor.h"
#include "arduino_time.h"


// DHT11 Sensor Object
#define DHT_TYPE           DHT11
DHT_Unified dht( A4, DHT_TYPE );

// DHT11 Sensor Context
temp_ctx_t temp_ctx;

// DHT11 UOM (temperature scale or units)
static temp_scale_t temp_scale = FAHRENHEIT_SCALE;


// DHT11 payload format. CBOR format if set to 1.
uint32_t temp_cbor_format = TEMP_CBOR_PAYLOAD;



//////////////////////////////////////////////////////////////////////////
//
// Code to handle inbound CoAP requests. This handler must be reference in the crarduino function
// located in mshield.ino (the resource request dispatcher).
//
//////////////////////////////////////////////////////////////////////////

/*
 * crtemperature
 *
 * @brief DHT11 CoAP resource specific code
 *
 */
error_t crtemperature(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it)
{
    struct optlv *o;
	uint8_t obs = false;


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
    }
    
    /*
     * GET for reading sensor or config information
     */
    else if (req->code == COAP_REQUEST_GET)
    {
        uint8_t rc, len = 0;

        /* Config values */
        /* GET /temp?cfg */
        if (!coap_opt_strcmp(o, "cfg"))
        {
            /* get temperature config */
            rc = arduino_get_temp_cfg( rsp->msg, &len );
        }
        else if (!coap_opt_strcmp(o, "sens"))
        {
	        /* Sensor values */
	        /* GET /temp?sens */
			if ((o = copt_get_next_opt_type((sl_co*)&(req->oh), COAP_OPTION_OBSERVE, NULL))) 
			{
				uint32_t obsval = co_uint32_n2h(o);
				switch(obsval)
				{
					case COAP_OBS_REG:
						rc = coap_obs_reg();
						obs = true;
						break;
					
					case COAP_OBS_DEREG:
						rc = coap_obs_dereg();
						break;
					
					default:
						rc = ERR_INVAL;
				}
			}
			else
			{
				/* Get sensor value */
				rc = arduino_get_temp( rsp->msg, &len );
			}
        }
        else
		{
            /* Don't support other queries. */
            rsp->code = COAP_RSP_501_NOT_IMPLEMENTED;
            goto err;
        }
		
        dlog(LOG_DEBUG, "crtemperature: GET status: %d len: %d bytes", rc, len);
        if (!rc)
		{
			if (obs)
			{                        
			/* Good code, but no content. */
				rsp->code = COAP_RSP_203_VALID;
				rsp->plen = 0;
			}
			else
			{
				rsp->plen = len;
				rsp->cf = COAP_CF_CSV;
				rsp->code = COAP_RSP_205_CONTENT;
			}
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
    }
    
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
} // crtemperature


//////////////////////////////////////////////////////////////////////////
//
// Code to build the CoAP payload. Temp payload is text with this format:
//   <epoch>,<temp>,<uom>
//     <epoch> is a 4 byte (8 hex char) timestamp
//     <temp> is a decimal number
//     <uom> is a char: C or F
//
//  Note that the payload is text. Payloads can also be a byte array of binary data.
//
//////////////////////////////////////////////////////////////////////////
error_t temp_build_payload(char *buf, uint32_t count, float *reading)
{
	char 		payload_buf[64];
	char		reading_buf[32];
	char		unit_buf[4];
	time_t     	epoch;
	uint32_t	indx;
	
	// Create string containing the UNIX epoch
	epoch = get_rtc_epoch();
	sprintf(payload_buf, "%d,", epoch);
	
	// Check if we have a sensor reading
	if (reading)
	{
		// Get each value
		for(indx = 0; indx < count; indx++)
		{
			// Create string containing reading and add it to the buffer
			sprintf(reading_buf, "%.2f,", *reading++);
			strcat(payload_buf, reading_buf);
		}
	}
	
	// Check UOM
	strcpy( unit_buf, "F" );
	if ( temp_scale == CELSIUS_SCALE )
	{
		// Change return unit to Celsius
		strcpy( unit_buf, "C" );
	}
	// Add UOM to payload
	strcat(payload_buf, unit_buf);
	
	strcpy(buf, payload_buf);
	
	dlog(LOG_DEBUG, "Text Payload: %s", payload_buf);
	return ERR_OK;
	
} // temp_build_payload


// This key denotes NIC type, which is used to determine how to store in MQTT broker.
#define NAMESPACE_NIC_TYPE_KEY          0
// Denotes device-specific data to follow in the CBOR payload
#define NAMESPACE_DEVICE_SPECIFIC_KEY   1
//
// Create the top level CBOR map and add the device type
//
int cbor_enc_nic_type(struct cbor_buf *cbuf, char *nic_type)
{
    int rcode;

    // Top level map, first element is for device type.
    // Device type used to provide a namespace for use at the MQTT broker.
    if ((rcode = cbor_enc_map(cbuf, 2)))
	{
        return rcode;
    }

    // NIC type, key and value
    if ((rcode = cbor_enc_int(cbuf, NAMESPACE_NIC_TYPE_KEY)))
	{
		return rcode;
    }
    if ((rcode = cbor_enc_text(cbuf, nic_type, strlen(nic_type))))
	{
		return rcode;
    }

    // Remaining device-specific key
    rcode = cbor_enc_int(cbuf, NAMESPACE_DEVICE_SPECIFIC_KEY);
    return rcode;
}


//////////////////////////////////////////////////////////////////////////
//
// Code to build the CoAP response message. Called on these CoAP requests:
//   Get temp
//   Observe temp
//   Get cfg
//
// The sensor payload returned in a response can be simple text (string) or a CBOR payload.
// The CBOR payload is a CBOR wrapper containing the device type followed by the text payload.
//
// A typical CBOR payload: {0:"temp",1:<text payload>"}
//
//////////////////////////////////////////////////////////////////////////
error_t temp_rsp_msg(struct mbuf *m, uint8_t *len, uint32_t count, float *reading)
{
	char		text_payload_buf[64];
	char		cbor_payload_buf[PAYLOAD_MAX_SIZE];
	char  		*p;
	uint8_t 	l;
	
	// Get text payload
	temp_build_payload(text_payload_buf, count, reading);
	
	
	// If payload needs to be in CBOR format we add the CBOR wrapper.
	if (temp_cbor_format)
	{
		error_t	rcode = ERR_FAIL;
		struct cbor_buf cbuf;
		
		cbor_enc_init(&cbuf, cbor_payload_buf, PAYLOAD_MAX_SIZE);
		
		char device_type[8];
		strcpy(device_type, CBOR_DEVICE_TYPE);
		if (!cbor_enc_nic_type(&cbuf, device_type))
		{
			if (!cbor_enc_text(&cbuf, text_payload_buf, strlen(text_payload_buf)))
			{
				rcode = ERR_OK;
			}
		}
		if (rcode == ERR_FAIL)
		{
			return rcode;
		}
		
		// Get CBOR payload length and allocate memory for the CBOR payload
		l = cbor_buf_get_len(&cbuf);
		p = (char *) m_append(m, l);
		if (!p)
		{
			return ERR_NO_MEM;
		}

		// Copy CBOR payload to response
		memcpy(p, cbor_payload_buf, l);
		*len = l;
		
		dlog(LOG_DEBUG, "CBOR Payload Dump:");
		ddump(LOG_DEBUG, "Payload", cbor_payload_buf, l);
	}
	// If Text payload than get payload length and allocate memory for the Text payload.
	else
	{
		l = strlen(text_payload_buf);
		p = (char *) m_append(m, l);
		if (!p)
		{
			return ERR_NO_MEM;
		}

		// Copy payload to response
		strcpy(p, text_payload_buf);
		*len = l;
		
		dlog(LOG_DEBUG, "Text Payload Dump:");
		ddump(LOG_DEBUG, "Payload", text_payload_buf, l);
	}
	
	int freeram = free_ram();
	dlog(LOG_DEBUG, "Free Ram: %d", freeram);
	return ERR_OK;
	
} // temp_rsp_msg


//////////////////////////////////////////////////////////////////////////
//
// DHT11 initialization code. Usually called in setup function.
//
//////////////////////////////////////////////////////////////////////////

/*
 * arduino_temp_sensor_init
 *
 * @brief Initialize DHT11 sensor
 *
 */
error_t arduino_temp_sensor_init()
{
	// Initialize temperature/humidity sensor
	dht.begin();

	// Enable temp sensor
	arduino_enab_temp();
	
	// Log a banner for the sensor
	println("DHT11 Sensor Initialized!");

	// Print DHT11 sensor details
	sensor_t sensor;
	dht.temperature().getSensor(&sensor);
	println("");
	println("------------------------------------");
	print  ("Sensor:       "); println(sensor.name);
	print  ("Driver Ver:   "); printnum(sensor.version);    println("");
	print  ("Unique ID:    "); printnum(sensor.sensor_id);  println("");
	print  ("Max Value:    "); printnum(sensor.max_value);  println(" C");
	print  ("Min Value:    "); printnum(sensor.min_value);  println(" C");
	print  ("Resolution:   "); printnum(sensor.resolution); println(" C");
	println("------------------------------------");        println("");
	return ERR_OK;
} // arduino_temp_sensor_init


//////////////////////////////////////////////////////////////////////////
//
// DHT11 specific code. These are helpers called by the above handlers.
//
//////////////////////////////////////////////////////////////////////////

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
    if (!rc)
	{
        coap_stats.sensors_disabled++;
    }
    return rc;
}


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
	}

	// Fahrenheit
	return 'F';
	
} // arduino_get_temp_scale


/**
 *
 * arduino_get_temp
 *
 * @brief Read temperature sensor and return a CoAP response message
 */
error_t arduino_get_temp( struct mbuf *m, uint8_t *len )
{
	const uint32_t count = 1;
	float reading = 0.0;
    error_t rc;

	// Check if disabled
	if (temp_ctx.state == tsat_disabled)
	{
		return ERR_OP_NOT_SUPP;
	}

	// Read temp sensor, already in network order
    rc = temp_sensor_read(&reading);
	if (rc != ERR_OK)
	{
		return rc;
	}
	
	// Assemble the CoAP response message
	rc = temp_rsp_msg(m, len, count, &reading);
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
       	println("Celsius scale set!");
		break;

	case FAHRENHEIT_SCALE:
		temp_scale = FAHRENHEIT_SCALE;
       	println("Fahrenheit scale set!");
		break;

	default:
		return ERR_INVAL;
	}

	// Enable
	arduino_enab_temp();
	return ERR_OK;

} // arduino_put_temp_cfg


/*
 * arduino_get_temp_cfg()
 *
 * Get temperature sensor scale
 */
error_t arduino_get_temp_cfg( struct mbuf *m, uint8_t *len )
{
    error_t rc;
	uint32_t count = 0;
	
	// Assemble the CoAP response message
	rc = temp_rsp_msg(m, len, count, NULL);

	return rc;
}


//////////////////////////////////////////////////////////////////////////
//
// DHT11 low level code. Sensor specific helpers.
//
//////////////////////////////////////////////////////////////////////////

error_t temp_sensor_read(float * p)
{
    error_t rc = ERR_OK;
	float re = INVALID_TEMP;

    if (temp_ctx.state == tsat_disabled)
    {
		println("Temp sensor disabled!");
        return ERR_OP_NOT_SUPP;
    }

	// Get temperature event and print its value.
	sensors_event_t event;  
	dht.temperature().getEvent(&event);

	// Check for NaN
	if (isnan(event.temperature)) 
	{
		println("Error reading temperature!");
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
