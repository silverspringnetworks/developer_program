/*

Copyright (c) Itron, Inc.
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

#ifndef PORTABILITY_H_
#define PORTABILITY_H_

//////////////////////////////////////////////////////////////////////////
//
// This file contains items needed to bridge the differences in different Arduino compatible hardware boards.
// In fact it applies to any ARM type MCU that adheres to the CMSIS Core and Runtime Environment.
//
// Use of hardware specific defines is allowed here!
//
// Note that this header should not contain any code (like class declarations). Code items should be located in
// the portability code layer: portability.cpp.
//
// Please contact the Itron Technical Enablement Team for details and help crafting portability to a specific board.
//
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
// Pin assignments. These typically index into the PORT assignment table. Chnage based on your hardware.
//
//////////////////////////////////////////////////////////////////////////

// The mshield analog pins
#define LIGHT_PIN			A3
#define DHT_PIN				A4

// The mshield digital pins
#define PIN_D3				3
#define MNIC_WAKEUP_PIN		PIN_D3


//////////////////////////////////////////////////////////////////////////
//
// Milli wake delay. The time in ms that we keep the wake pin low. Signals the Milli to start an HDLC exchange.
//
//////////////////////////////////////////////////////////////////////////
#define MILLI_WAKE_DELAY	2


//////////////////////////////////////////////////////////////////////////
//
// Serial and USB
// 
// Not all hardware vendors USB uses the standard core. Account for the differences here.
//
//////////////////////////////////////////////////////////////////////////

// For the STM32 L4XX series boards, STM does not use the standard core. In the STM core USB and HardwareSerial objects are reversed.
// Thus Serial_ does not work as expected in the logging code.
#if defined(STM32L4xx)
#define PORTABILITY_SERIAL_USB HardwareSerial
#else
#define PORTABILITY_SERIAL_USB Serial_
#endif


//////////////////////////////////////////////////////////////////////////
//
// Serial and USB Associations.
//
// These are used by the CoAP Server. Set the Serial and USB association for you board.
//
//////////////////////////////////////////////////////////////////////////

// For Boards that use a Atmel SAMD chip but are not the ADAFRUIT_METRO_M0_EXPRESS board
#ifndef ADAFRUIT_METRO_M0_EXPRESS

// For Boards that use other Atmel SAMD chip family
#if defined(ARDUINO_ARCH_SAMD)
#define SER_MON_PTR					&SerialUSB
#define UART_PTR        			&Serial1
#endif

// For Boards that use other Atmel SAM chip families
#if defined(ARDUINO_ARCH_SAM)
#define SER_MON_PTR				  	&SerialUSB
#define UART_PTR        			&Serial
#endif
#endif

// For the AdaFruit Metro M0 Express (uses the SAMD chip family).
#if defined(ADAFRUIT_METRO_M0_EXPRESS)
#define SER_MON_PTR				    &Serial
#define UART_PTR				    &Serial1
#endif

// For Boards that use the Atmel SAML chip families
#if defined(SAML21)
#define SER_MON_PTR					&SerialUSB
#define UART_PTR        			&Serial2
#endif

// For STM Boards that use the STM32 L4xx chip family
#if defined(STM32L4xx)
#define SER_MON_PTR					&Serial
#define UART_PTR        			&Serial4
#endif


#endif /* PORTABILITY_H_ */