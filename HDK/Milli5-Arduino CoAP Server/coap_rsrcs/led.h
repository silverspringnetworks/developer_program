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

#ifndef LED_H_
#define LED_H_

#include <arduino.h>
#include "errors.h"

typedef enum
{
	LED_ON,
	LED_OFF,
	LED_BLINK_SLOW,
	LED_BLINK_FAST,
	LED_DISABLE,
	NUM_LED_STATES
	
} led_state_t;

typedef struct
{
	uint32_t 	led_pin;
	uint32_t	led_delay;
	uint32_t	led_slow;
	uint32_t	led_fast;
	led_state_t	led_state;
	boolean 	actuate_flag;
	
} led_config_t;

/******************************************************************************/
/*                      Public Methods                                        */
/******************************************************************************/

/*
 * @brief CoAP Resource external LED
 *
 */
error_t crled( struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, void *it );

/**
 *
 * @brief Config LED
 *
 * @param[in] state Desired LED state
 * @return error_t
 *
 */
void led_config( uint32_t led_pin, uint32_t blink_seconds, uint32_t ms_slow, uint32_t ms_fast );

/**
 *
 * @brief Blink LED
 *
 *
 */
error_t led_blink();

/**
 * @brief Put LED state
 *
 */
error_t arduino_put_led_state( led_state_t state );

/**
 * @brief Get LED state
 *
 * @param[in] m Pointer to output mbuf
 * @param[in] len Length of output
 * @return error_t
 */
error_t arduino_get_led_state( struct mbuf *m, uint8_t *len );

/*
 * @brief Turn LED on or off, blink fast/slow or disable the LED
 */
error_t arduino_led_actuate();

/******************************************************************************/
/*                     Private Methods                                        */
/******************************************************************************/

#endif /* LED_H_ */
