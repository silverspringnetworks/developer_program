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
#ifndef ARDUINO_PINS_H_
#define ARDUINO_PINS_H_

#include <arduino.h>

/**
 * @brief Defines for Arduino analog inputs
 *
 * @enum arduino_analog_in_t
 *
 */
typedef enum
{
	A0_RESERVED	= A0,
	A1_RESERVED = A1,
	A2_RESERVED = A2,
	LIGHT_PIN	= A3,
	DHT_PIN		= A4,
	A5_RESERVED = A5,
	NUM_ANALOG_IN = 6
	
} arduino_analog_in_t;

/**
 * @brief Defines for Arduino digital pins
 *
 * @enum arduino_digital_pin_t
 *
 */
typedef enum
{
	PIN0_RESERVED,		// Pin 0
	PIN1_RESERVED,		// Pin 1
	PIN2_RESERVED,		// Pin 2
	MNIC_WAKEUP_PIN,	// Connected to MILLI_5_WAKEUP/GPIO_7
	PIN4_RESERVED,		// Pin 4
	PIN5_RESERVED,
	PIN6_RESERVED,
	PIN7_RESERVED,
	PIN8_RESERVED,
	PIN9_RESERVED,
	PIN10_RESERVED,
	PIN11_RESERVED,
	PIN12_RESERVED,
	ONBOARD_LED = LED_BUILTIN,
	DIGITAL_PINS_COUNT
	
} arduino_digital_pin_t;

#endif /* ARDUINO_PINS_H_ */
