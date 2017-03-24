#include <arduino_pins.h>
#include <bufutil.h>
#include <errors.h>
#include <hbuf.h>
#include <includes.h>
#include <log.h>

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

#ifndef _MSHIELD_H_
#define _MSHIELD_H_

#include "Arduino.h"

#if defined(ARDUINO_ARCH_SAMD)
  // Use SerialUSB for Serial on Zero based boards
  #define Serial SerialUSB
#endif

// Local time zone relative to UTC; Examples PST: -8, MST: -7, Central: -6, EST: -5
#define LOCAL_TIME_ZONE					(-8)

// UART between Arduino and MilliShield
#define UART_BAUD_RATE        			38400
#define UART_TIMEOUT_IN_MILLISECONDS	2000

// LED parameters
#define LED_BLINK_SLOW_PERIOD			800
#define LED_BLINK_SLOW_COUNT			7
#define LED_BLINK_FAST_PERIOD			100
#define LED_BLINK_FAST_COUNT			50

// Max-Age, see Section 5.10.5 of rfc7252
#define COAP_MSG_MAX_AGE_IN_SECONDS		90

#endif /* _MSHIELD_H_ */
