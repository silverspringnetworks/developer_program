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
#ifndef TEMPLATE_COAP_RESOURCE_H_
#define TEMPLATE_COAP_RESOURCE_H_

#include <arduino.h>
#include "errors.h"
#include "hbuf.h"

typedef struct TT_cfg_struct
{
	uint32_t state;
	
} TT_cfg_t;

/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/

/**
 *
 * @brief CoAP Resource "put description here"
 *
 */
error_t crmysensor(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it);

/**
 * @brief Enable sensor
 *
 * @return error_t
 */
error_t arduino_enab_TT(void);

/**
 * @brief Disable sensor
 *
 * @return error_t
 */
error_t arduino_disab_TT(void);

/**
 * @brief Init sensor
 * @return error_t
 *
 */
error_t arduino_init_TT();

/**
 * @brief Set TT state
 * @param[in] state Desired state
 * @return error_t
 */
error_t arduino_set_TT_state( uint32_t state );

/**
 * @brief Get TT state
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_TT_state(struct mbuf *m, uint8_t *len);

/**
 * @brief Set TT config
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_set_TT_cfg(struct mbuf *m, uint8_t *len);

/**
 * @brief Get TT config
 * @param[in] m Pointer to input mbuf
 * @param[in] len Length of input
 * @return error_t
 */
error_t arduino_get_TT_cfg(struct mbuf *m, uint8_t *len);


/**
 * @brief A function that reads a sensor and assembles a return message
 * @param[out] m Pointer to return message
 * @param[out] len Length of return message
 * @return error_t
 * 
 */
error_t arduino_get_TT(struct mbuf *m, uint8_t *len);


/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/

/**
 * @brief
 * @return error_t
 */
error_t TT_enable(void);

/**
 * @brief
 * @return error_t
 */
error_t TT_disable(void);

/**
 * @brief Read sensor 
 * @return error_t
 */
error_t TT_read( float * p );

#endif /* TEMPLATE_COAP_RESOURCE_H_ */
