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


/*!
 *	The driver for the Echo Sensor. The sapi_echo application shows how to use
 *	a non sensor sensor (that is there is there is no sensor hardware).
 *
 *	Load it up and use it in Arduino or Atmel Studio in the standard way.
*/

#include <Arduino.h>
#include "sapi.h"
#include "EchoSensor.h"


static uint8_t echo_sensor_id;


//
//  Arduino setup function.
//
void setup()
{
	sapi_error_t rcode;
	
	// Initialize Sensor API
	sapi_initialize(NULL);
	
	// Register echo sensor. Note the sensor does not support cfg read.
	echo_sensor_id = sapi_register_sensor(ECHO_SENSOR_TYPE, echo_init_sensor, echo_read_sensor, NULL, echo_write_cfg, 1, 90);
	
	// Initialize echo sensor
	rcode = sapi_init_sensor(echo_sensor_id);
}


//
// Arduino main loop function.
//
void loop()
{
	// Call SAPI run to do the heavy lifting
	sapi_run();
}
