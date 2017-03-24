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

/*! \mainpage
 *
 * Copyright (C) Silver Spring Networks Inc. 2017 <br>
 * 
 *
 * \section intro_sec Introduction
 *
 * These Arduino MilliShield libraries were written and tested for Arduino M0 Pro <br>
 * This code base consists of three Arduino libraries: <br>
 *
 * \subsection hdlc HDLC
 * \subsection coap CoAP Server
 * \subsection log Log
 *
 * The Arduino setup() and loop() functions are in the example sketch mshield.ino <br>
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Copy the three libraries to C:\Users\<user>\Documents\Arduino\libraries
 * \subsection step2 Step 2: In the Arduino IDE, go to Sketch, Include Library and select HDLC
 * \subsection step3 Step 3: In the Arduino IDE, go to Sketch, Include Library and select CoAP Server
 * \subsection step4 Step 4: In the Arduino IDE, go to Sketch, Include Library and select Log
 *
 * \note {Replace <user> with your Windows user name.}
 *
 */
 
#include "mshield.h"
#include "coap_server.h"
#include "hdlcs.h"
#include "log.h"
#include "resrc_coap_if.h"
#include "coapsensoruri.h"
#include "led.h"

#define VERSION_NUMBER "1.2.4"

// Specify baud rate for the debug console
#define CONSOLE_BAUD_RATE     115200

// Specify pointer to Serial object used to  
// access the UART for the HDLC connection
// NOTE: the baud rate of the HDLC connection is fixed
#define UART_SERIAL           &Serial1

// CoAP server instatiation
static CoAP_Server coap;

// The Arduino init function
void setup()
{
  char ver[64];
  int res;

  // Set-up serial port for debug output
  // Print debug message via Native USB
  // NOTE: it takes a few seconds before you can start printing
  Serial.begin(CONSOLE_BAUD_RATE);
		
	// Init CoAP registry
	coap_registry_init();

  /* 
   * Since coap_s_uri_proc only matches on the first path option, and it's
   * against the whole path, registering L_URI_LOGISTICS guarantees a match
   * or that URI, and its children. Putting it first means it's found faster.
   * We will never match the subsequent L_URI_LOGISTICS paths due to the
   * separation of the options but the concatenation of the path.
   * TODO Optimise this so that only one URI is required to be registered
   * while satisfying .well-known/core dicovery.
   */
  coap_uri_register(L_URI_ARDUINO, crarduino, CLA_ARDUINO);

  // Init the Arduino resources such as sensors, LEDs etc.
  // This will blink external LED while waiting for console port to start
  arduino_init_resources();
  
	// Open HDLCS
  res = hdlcs_open(UART_SERIAL);
  if (res) 
	{
    dlog(LOG_ERR, "HDLC initialization failed");
    
  } // if

  // Print version number, time and date
  sprintf( ver, "Arduino MilliShield Software Version Number: %s", VERSION_NUMBER );
  Serial.println(ver);
  sprintf( ver, "%s %s\n", __TIME__, __DATE__ );
  Serial.println(ver);
  
} // setup


// The main loop
void loop()
{
	struct mbuf *appd;
	struct mbuf *arsp;

  /* Only actuate if disconnected */
  if ( false == hdlcs_is_connected() )
  {
      // Actuate LED
      arduino_led_actuate();
      
  } // if
  
	/* Run the secondary-station HDLC state machine */
	hdlcs_run();
	
	/* Serve incoming request, if any */
	appd = hdlcs_read();
	if (appd) 
	{
    /* Run the CoAP server */
		arsp = coap.s_proc(appd);
		if (arsp) 
		{
      /* Send CoAP response, if any */
		  hdlcs_write(arsp->data, arsp->len);
     
		} // if
		
	} // if
} // loop

