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
 *
 * \section intro_sec Introduction
 *
 * These Arduino MilliShield libraries were written and tested for Arduino M0 Pro <br>
 * This code base consists of three Arduino libraries: <br>
 *
 * \subsection hdlc The HDLC library
 * \subsection coap_server The CoAP Server library
 * \subsection coap_rsrcs Library of CoAP resources; sensors, time, etc.
 * \subsection utils Logging, buffer management etc.
 *
 * The Arduino setup() and loop() functions are in the example sketch mshield.ino <br>
 *
 * \section install_sec Installation
 *
 * \subsection step1 Step 1: Copy the four libraries to C:\Users\<user>\Documents\Arduino\libraries
 * \subsection step2 Step 2: In the Arduino IDE, go to Sketch, Include Library, Contributed Libraries
 * \subsection step3 Step 3: Select hdlc
 * \subsection step4 Step 4: Repeat Step 3 for coap_server, coap_rsrcs and utils
 * \subsection step5 Step 5: In the IDE, go to Sketch/Include Library/Manage Libraries
 * \subsection step6 Step 6: In the search bar, search for RTCZero and install that library
 * \subsection step7 Step 7: Do you have the DHT11 sensor? 
 * \subsection step8 Step 7a: If yes, use Manage Libraries to locate and install the following libraries:
 * \subsection step9 Step 7b: Adafruit Unified Sensor and DHT Sensor Library
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
#include "arduino_time.h"

#define VERSION_NUMBER "1.2.5"

// The Arduino init function
void setup()
{
  char ver[64];
  int res;

  // Set-up serial port for logging output
  // Print debug message via Native USB
  // NOTE: it takes a few seconds before you can start printing
  // The object Serial is defined in mshield.h
  log_init(&Serial,CONSOLE_BAUD_RATE);

  // Set local time zone
  set_time_zone(LOCAL_TIME_ZONE);
		
	// Init CoAP registry
	coap_registry_init();

  /*
   * Since coap_s_uri_proc only matches on the first path option, and it's
   * against the whole path, registering L_URI_LOGISTICS guarantees a match
   * or that URI, and its children. Putting it first means it's found faster.
   * We will never match the subsequent L_URI_LOGISTICS paths due to the
   * separation of the options but the concatenation of the path.
   * TODO Optimise this so that only one URI is required to be registered
   * while satisfying .well-known/core discovery.
   */
  coap_uri_register(L_URI_ARDUINO, crarduino, CLA_ARDUINO);

  /* Set Max-Age; CoAP Server Response Option 14 */
  coap_set_max_age(COAP_MSG_MAX_AGE_IN_SECONDS);

  /* Configure LED */
  led_config( LED_PIN_NUMBER, LED_BLINK_DURATION_IN_SECONDS, LED_BLINK_SLOW_PERIOD_MS, LED_BLINK_FAST_PERIOD_MS );
  
  // Init the Arduino resources such as sensors, LEDs etc.
  // This will blink the external LED for a few seconds
  // After this, you should be able to print to Serial
  arduino_init_resources();
  
	// Open HDLCS
  // The object SerialUART is defined in mshield.h
  res = hdlcs_open( &SerialUART, UART_TIMEOUT_IN_MILLISECONDS );
  if (res) 
	{
    dlog(LOG_ERR, "HDLC initialization failed");
    
  } // if

  // Print version number, time and date
  sprintf( ver, "Arduino MilliShield Software Version Number: %s\n", VERSION_NUMBER );
  print_buf(ver);
  sprintf( ver, "Time: %s\n", __TIME__ );
  print_buf(ver);
  sprintf( ver, "Date: %s\n", __DATE__ );
  print_buf(ver);
  
} // setup

// The main loop
void loop()
{
	struct mbuf *appd;
	struct mbuf *arsp;
  boolean is_con;

  /* Only actuate if disconnected */
  is_con = hdlcs_is_connected();
  if ( false == is_con )
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
		arsp = coap_s_proc(appd);
		if (arsp) 
		{
      /* Send CoAP response, if any */
		  hdlcs_write(arsp->data, arsp->len);
     
		} // if
		
	} // if
} // loop

