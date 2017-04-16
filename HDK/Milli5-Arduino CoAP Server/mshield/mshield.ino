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
 * These Arduino MilliShield libraries were written and tested for Arduino ZERO Pro, M0 Pro, Zero and Due <br>
 * This code base consists of three Arduino libraries: <br>
 *
 * \subsection coap_server The CoAP Server library
 *
 * The Arduino setup() and loop() functions are in the example sketch mshield.ino <br>
 *
 * \section install_sec Installation
 *
 * \subsection step1  Step 1:  Download and install Arduino IDE 1.8.2 from https://www.arduino.cc/en/Main/Software or http://www.arduino.org/downloads depending on your Arduino hardware:
 * \subsection step2  Step 2:  Copy the folder mshield to anywhere on your system e.g. c:\mshield
 * \subsection step3  Step 3:  Copy the folder ssni_coap_sensor to C:\Users\<user>\Documents\Arduino\libraries  Replace <user> with your username.
 * \subsection step4  Step 4:  In the folder mshield, click on mshield.ino and your Arduino IDE will launch
 * \subsection step5  Step 5:  In the Arduino IDE, go to Sketch, Include Library, Contributed Libraries and select ssni_coap_server
 * \subsection step6  Step 6a: In the IDE, go to Sketch/Include Library/Manage Libraries
 * \subsection step7  Step 6b:   In the search bar, type RTCZero or RTCDue depending on what kind of hardware you have
 * \subsection step8  Step 6c:   Click on "More info"
 * \subsection step9  Step 6d:   In the drop-down menu, select v1.5.2 and click Install
 * \subsection step10 Step 7a: Do you have the DHT11 sensor? 
 * \subsection step11 Step 7b:   If yes, use Manage Libraries to locate and install the following libraries:
 * \subsection step12 Step 7c:     Adafruit Unified Sensor v1.0.2
 * \subsection step13 Step 7d:     Adafruit DHT Sensor v1.3.0
 *
 * \note {Replace <user> with your Windows user name.}
 *
 */
 
#include "mshield.h"


/*
 * This function handles the URI prior to the query
 * Add your own URI here
 */
error_t crarduino( struct coap_msg_ctx *req, struct coap_msg_ctx *rsp )
{
    struct optlv *o;
    void *it = NULL;

    /* 
     * No URI path beyond /arduino, except /temp is supported, so reject if present. 
     * 
     */
    copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it);
    if ((o = copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_URI_PATH, &it))) 
    {
        // This is the default URI
        if (!coap_opt_strcmp( o, TEMP_SENSOR ))
        {
            return crtemperature( req, rsp, it );
        }

        
        /* Below, replace MY_SENSOR with your own name of your particular sensor  */
        /* Use the enclosed template (TT_resource.cpp and TT_resource.h) to       */
        /* implement the crmysensor function and associated methods               */
        //if (!coap_opt_strcmp( o, MY_SENSOR ))
        //{
        //    /* Replace mysensor below with a name for your sensor               */
        //    /* The function 'crmysensor' is implemented in a new C++ file       */
        //    return crmysensor( req, rsp, it );
        //}
        
        rsp->code = COAP_RSP_404_NOT_FOUND;
    
    } // if            

    rsp->plen = 0;

    return ERR_OK;
  
} // crarduino

/********************************************************************************/

// The Arduino init function
void setup()
{
  // Set-up serial port for logging output
  // Print debug messages to the Serial Monitor
  // The pointer to the serial object is defined in mshield.h
  // NOTE: it takes a few seconds before you can start printing
  log_init( SER_MON_PTR, SER_MON_BAUD_RATE, LOG_LEVEL );

  // Init the clock and set the local time zone
  rtc_time_init(LOCAL_TIME_ZONE);

  // Init the temp sensor
  arduino_temp_sensor_init();

  // Init the CoAP Server
  coap_s_init( UART_PTR, COAP_MSG_MAX_AGE_IN_SECS, UART_TIMEOUT_IN_MS, OBS_SENSOR_NAME, OBS_FUNC_PTR );
}

/********************************************************************************/

// The main loop
void loop()
{
  // Run CoAP Server
  coap_s_run();
}

/********************************************************************************/

