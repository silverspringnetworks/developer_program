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

#ifndef XYZ_SENSOR_H_
#define XYZ_SENSOR_H_

#include <arduino.h>
#include "errors.h"

typedef struct xyz_sensor_cfg_struct
{
	uint32_t state;
	
} xyz_sensor_cfg_t;

/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/

/*
 * crxyz
 *
 * @brief CoAP Resource XYZ sensor
 *
 */
error_t crxyz( struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it );

/*
 * arduino_disab_xyz
 *
 * @brief
 *
 */
error_t arduino_disab_xyz();

/*
 * arduino_enab_xyz
 *
 * @brief
 *
 */
error_t arduino_enab_xyz();

/*
 * @brief XYZ magnetometer sensor init
 *
 */
error_t arduino_xyz_sensor_init();

/**
 * @brief Get XYZ magnetometer sensor 
 *
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_xyz(struct mbuf *m, uint8_t *len);

/*
 * @brief Get XYZ magnetometer sensor state
 * @return error_t
 */
error_t arduino_get_xyz_state(struct mbuf *m, uint8_t *len);



/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/

/*
 * xyz_sensor_read
 *
 * @brief
 *
 */
error_t xyz_sensor_read( int * pX, int * pY, int * pZ );


/*
 * @brief
 * @return error_t
 */
error_t xyz_sensor_enable(void);

/*
 * @brief
 */
error_t xyz_sensor_disable(void);

#endif /* XYZ_SENSOR_H_ */
