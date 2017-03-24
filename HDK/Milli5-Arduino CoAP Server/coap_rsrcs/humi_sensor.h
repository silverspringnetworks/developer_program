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

#ifndef HUMI_SENSOR_H_
#define HUMI_SENSOR_H_

#include "errors.h"

/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/

/*
 * crhumidity
 *
 * @brief CoAP Resource humidity sensor
 *
 */
error_t crhumidity( struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it );

typedef struct humi_sensor_cfg
{
    int8_t  humi_max_thres;
    int8_t  humi_min_thres;
    uint8_t  humi_hyst;
    uint8_t  state;
	
} humi_sensor_cfg_t;

/*
 * @brief  Get humidity sensor state
 * @return error_t
 */
error_t arduino_get_humi_state( struct mbuf *m, uint8_t *len );

/*
 * @brief Init humidity sensor
 *
 */
error_t arduino_humi_sensor_init();

/**
 * @brief Get sensor humidity
 *
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_humi(struct mbuf *m, uint8_t *len);


/**
 * @brief Enable humidity sensor
 *
 * @return error_t
 */
error_t arduino_enab_humi(void);

/**
 * @brief Disable humidity sensor
 *
 * @return error_t
 */
error_t arduino_disab_humi(void);

/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/

/*
 * humi_sensor_read
 *
 * @param p if error_t is ERR_OK, humidity reading will be returned.
 * @return error_t
 *
 */
error_t humi_sensor_read( float * p );

/*
 * @brief  Enable humidity sensor
 * @return error_t
 */
error_t humi_sensor_enable(void);

/*
 * @brief  Disable humidity sensor
 * @return error_t
 */
error_t humi_sensor_disable(void);

#endif /* HUMI_SENSOR_H_ */
