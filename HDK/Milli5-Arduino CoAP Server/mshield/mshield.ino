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

   \section intro_sec Introduction

   The mshield Arduino application and associated library provides a reference CoAP Server
   for use with the Itron Starfish network and the milli shield hardware.
   Adding the milli shield to the appropriate Arduino board provides the basis for building out
   IoT applications.
   The mshield application was written for the
   Arduino M0 Pro board and the Adafruit Metro M0 Express board.<br>

   The code release consists of the ssni_coap_server library and an Arduino sketch.<br>

   \subsection coap_server The CoAP Server library

   The CoAP Server library is packaged as ssni_coap_server. It contains a basic CoAP server that supports
   the Itron milli chip and Arduino milli shield product and a variety of demonstration sensors.
   
   \subsection The Arduino Sketch

   The Arduino setup() and loop() functions are located in mshield.ino and mshield.h.
   Review the mshield.h as it contains valuable information.<br>

   \section install_sec Installation
   \subsection step1  Step 1:  Download and install Arduino IDE 1.8.7 from https://www.arduino.cc/en/Main/Software
   \subsection step2  Step 2:  In the preferences, locate the field "Additionial Board Manager URLs". Add this URL to the list: https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
   \subsection step3  Step 3:  Using the board manager, add the Adafruit SAMD boards. Select the Adafruit Metro M0 Express board.
   \subsection step4  Step 4:  From the mshield distribution (zip file), copy the sub-folder mshield to C:\Users\<user>\Documents\Arduino.
   \note {Replace <user> with your Windows user name.}
   \subsection step5  Step 5:  Copy the folder ssni_coap_server to C:\Users\<user>\Documents\Arduino\libraries.
   \subsection step6  Step 6:  In the folder Arduino\mshield, click on mshield.ino and your Arduino IDE will launch
   \subsection step7  Step 7:  In the IDE, go to the Sketch menu and choose Include Library, and select ssni_coap_server from the Contributed Libraries section
   \subsection step8  Step 8a: In the IDE, go to Sketch/Include Library/Manage Libraries
   \subsection step9  Step 8b:   In the search bar, type RTCZero
   \subsection step10  Step 8c:  Click on "More info"
   \subsection step11  Step 8d:  In the drop-down menu, select v1.5.2 and click Install
   \subsection step12 Step 9a: The following libraries are needed to support the DHT-11 temp sensor (these are needed even if you do not hook a DHT-11 up):
   \subsection step14 Step 9b:   Adafruit Unified Sensor v1.0.2
   \subsection step15 Step 9c:   DHT Sensor Library v1.3.0
   \subsection step16 Step 10:  Build and upload the application to your Adafruit Metreo M0 Express board. Explore and have fun!
*/


#include "mshield.h"


//////////////////////////////////////////////////////////////////////////
//
// Code to support registration of resource URI's. This function is registered for call back
// in the coap_registry_init function (see coapsensoruri.cpp). Modify this crarduino function to
// represent your sensor.
//
//////////////////////////////////////////////////////////////////////////
error_t crarduino( struct coap_msg_ctx *req, struct coap_msg_ctx *rsp )
{
  struct optlv *o;
  void *it = NULL;

  // Isolate the leaf of the URI. Do this by skipping CoAP options.
  copt_get_next_opt_type((const sl_co*) & (req->oh), COAP_OPTION_URI_PATH, &it);
  if ((o = copt_get_next_opt_type((const sl_co*) & (req->oh), COAP_OPTION_URI_PATH, &it)))
  {

    // Is the leaf the "temp" sensor?
    if (!coap_opt_strcmp( o, TEMP_SENSOR ))
    {
      return crtemperature( req, rsp, it );
    }


	// To add additional sensors, replace ONE_SENSOR with the sensor name as defined
	// in mshield.h. Use the enclosed template files (TT_resource.cpp and TT_resource.h) to
	// implement the "crmysensor" function and associated methods.

    //if (!coap_opt_strcmp( o, ONE_SENSOR ))
    //{
    //    /* Replace mysensor below with a name for your sensor               */
    //    /* The function 'crmysensor' is implemented in a new C++ file       */
    //    return crmysensor( req, rsp, it );
    //}


	// No URI path is supported, so reject with not found
    rsp->code = COAP_RSP_404_NOT_FOUND;
  }

  rsp->plen = 0;
  return ERR_OK;
} // crarduino


/*
 * setup
 *
 * @brief The Arduino runtime setup function
 */
void setup()
{
  // Set-up serial port for logging output
  // Print debug messages to the Serial Monitor
  // The pointer to the serial object is defined in mshield.h
  // NOTE: it takes a few seconds before you can start printing
  log_init( SER_MON_PTR, SER_MON_BAUD_RATE, LOG_LEVEL );

  // Init the clock and set the local time zone
  rtc_time_init(LOCAL_TIME_ZONE);

	// Log Banner: version number, time and date
	char ver[64];
	strcpy(ver, COAP_SERVER_VERSION_STRING);
	strcat(ver, COAP_SERVER_VERSION_NUMBER);
	println(ver);
	sprintf(ver, "Build Time: %s  Date: %s", __TIME__, __DATE__);
	println(ver);
	println("");

  // Init the temp sensor
  arduino_temp_sensor_init();

  // Init the CoAP Server
  coap_s_init( UART_PTR, COAP_MSG_MAX_AGE_IN_SECS, UART_TIMEOUT_IN_MS, MAX_HDLC_INFO_LEN, OBS_SENSOR_NAME, OBS_FUNC_PTR );

  int freeram = free_ram();
  dlog(LOG_DEBUG, "Free Ram: %d", freeram);
  
  // Send the reboot event to the mnic
  coap_put_ic_reboot_event();
  delay(50);
} // setup


/*
 * loop
 *
 * @brief The Arduino runtime main loop
 */
void loop()
{
  // All we do here is to call the CoAP Server run function!
  coap_s_run();
} // loop
