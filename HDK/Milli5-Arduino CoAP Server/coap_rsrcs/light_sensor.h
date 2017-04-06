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

#ifndef _LIGHT_SENSOR_H_
#define _LIGHT_SENSOR_H_

#include <arduino.h>
#include "errors.h"
#include "hbuf.h"

typedef struct light_cfg_struct
{
	uint32_t state;
	
} light_sensor_cfg_t;

/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/

/*
 * @brief CoAP Resource light sensor
 *
 */
error_t crlight( struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it );

/**
 * @brief Enable sensor
 *
 * @return error_t
 */
error_t arduino_enab_light(void);

/**
 * @brief Disable sensor
 *
 * @return error_t
 */
error_t arduino_disab_light(void);

/*
 * @brief
 * @return error_t
 *
 */
error_t arduino_light_sensor_init();

/**
 * @brief Get light sensor state
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_light_state(struct mbuf *m, uint8_t *len);

/**
 * @brief Set light sensor config
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_set_light_cfg(struct mbuf *m, uint8_t *len);

/**
 * @brief Get light sensor config
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_light_cfg(struct mbuf *m, uint8_t *len);

/**
 * @brief Get sensor 
 *
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_light(struct mbuf *m, uint8_t *len);


/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/

/**
 * @brief
 * @return error_t
 */
error_t light_sensor_enable(void);

/**
 * @brief
 */
error_t light_sensor_disable(void);

/**
 * @brief Read sensor 
 *
 * @return error_t
 */
error_t light_sensor_read( float * p );

#endif /* _LIGHT_SENSOR_H_ */
