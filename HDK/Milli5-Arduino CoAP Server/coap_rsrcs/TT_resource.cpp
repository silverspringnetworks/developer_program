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
#include "coapmsg.h"
#include "coappdu.h"
#include "arduino_pins.h"
#include "resrc_coap_if.h"
#include "TT_resource.h"


/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/

// TT Config
TT_cfg_t TT_cfg;

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
	// Set pointer to Serial object
	// pS is a static declared in log.h
	// Serial is defined in log.h
	pS = log_get_serial();

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


/*
 * @brief Get config
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 * 
 */
error_t arduino_get_TT( struct mbuf *m, uint8_t *len )
{
    error_t rc = ERR_OK;
	float reading;
	rc = TT_read(&reading);
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
    error_t rc = ERR_OK;
    return rc;
}

