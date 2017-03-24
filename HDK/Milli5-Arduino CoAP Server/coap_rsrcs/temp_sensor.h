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

#ifndef TEMP_SENSOR_H_
#define TEMP_SENSOR_H_

#include "errors.h"

typedef enum
{
	INVALID_TEMP = -100,
	CELSIUS_SCALE = 0x43,
	FAHRENHEIT_SCALE = 0x46,
	NUMBER_OF_SCALES = 2

} temp_scale_t;

typedef struct temp_sensor_cfg
{
    int8_t  temp_max_thres;
    int8_t  temp_min_thres;
    uint8_t  temp_hyst;
    uint8_t  enable;
} temp_sensor_cfg_t;


typedef enum {
    tsat_disabled = 0,
    tsat_no_change,
    tsat_cleared,
    tsat_high,
    tsat_low,
    tsat_err
} temp_sensor_alert_t;

typedef struct temp_ctx 
{
    temp_sensor_cfg_t	cfg;
    temp_sensor_alert_t state;
    
} temp_ctx_t;

/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/


/*
 * crtemperature
 *
 * @brief CoAP Resource temperature sensor
 *
 */
error_t crtemperature( struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it );

/*
 * disab_temp
 *
 * @brief
 *
 */
error_t arduino_disab_temp();

/*
 * enab_temp
 *
 * @brief
 *
 */
error_t arduino_enab_temp();

/*
 * temp_sensor_init
 *
 *
 */
error_t arduino_temp_sensor_init();

/*
 * @brief Get sensor temperature measurement unit
 *
 * Get temperature scale; Celsius or Fahrenheit
 */
char arduino_get_temp_scale();

/**
 * @brief Get sensor temperature
 *
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_temp(struct mbuf *m, uint8_t *len);

/**
 * @brief CoAP put temp config
 *
 * @param[in] scale Celsius or Fahrenheit
 * @return error_t
 */
error_t arduino_put_temp_cfg( temp_scale_t scale );

/**
 * @brief CoAP Server get temp config
 *
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_temp_cfg(struct mbuf *m, uint8_t *len);


/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/

/*
 * temp_sensor_read
 *
 * @param p: if error_t is ERR_OK, temperature reading will be returned.
 *
 */
error_t temp_sensor_read(float * p);


/*
 * @brief  Get thresholds for temp sensor alerts
 * @return error_t
 */
error_t temp_sensor_cfg_set(const struct temp_sensor_cfg *cfg);

/*
 * @brief  Enable temp alert XXX must be called in app task!
 * @return error_t
 */
error_t temp_sensor_enable(void);

/*
 * @brief  Disable temp alert XXX must be called in app task!
 * @return error_t
 */
error_t temp_sensor_disable(void);



#endif /* TEMP_SENSOR_H_ */
