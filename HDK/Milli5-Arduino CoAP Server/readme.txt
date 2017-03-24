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

==================================================
|        Arduino MilliShield Software            |
==================================================

This deliverable consists of four sub-folders:

1. mshield 		- Contains an Arduino sketch that uses the MilliShield libraries
2. hdlc 		- the HDLC library
3. coap_server 	- the CoAP Server library
4. coap_rsrcs	- library of CoAP resources; sensors, time, etc.
4. utils 		- the Utils library containing logging and buffer management

For installation instructions;

https://zoo.silverspringnet.com/display/platform/Set-up+and+Install
https://portal7-stage.mitangi.com/milli5-arduino-dev-kit-getting-started-guide

Release Notes:
++++++++++++++

Arduino MilliShield Software Version Number: 1.2.4
==================================================
- Added legal statement to all source files
- Added #define for local timezone in mshield.h
- Changed Max-Age to 90 seconds

Arduino MilliShield Software Version Number: 1.2.3
==================================================
- Ported Option 14, Max-Age, from the logistics code.
- Blink LED in dis-connected state only.


Arduino MilliShield Software Version Number: 1.2.2
==================================================
Made a tiny ISR that sets a flag at whole minutes. We check for the flag and 
if it is set, we assemble a response and wake-up the MilliShield.


Arduino MilliShield Software Version Number: 1.2.1
==================================================
- CoAP Observe on /sensor/arduino/temp?sens works 1-5 observations and then stops.
- The temperature sensor is disabled in this version and you'll get 0 Fahrenheit.
- The LED pin was moved from Pin 3 to Pin 4 because MNIC_WAKEUP_PIN is connected
to Pin 3. 


Arduino MilliShield Software Version Number: 1.2.0
==================================================
- Added support for XYZ Magnetometer and Light sensors.
- Template for adding new sensors/resources; TT_resource.cpp/TT_resource.h 


Arduino MilliShield Software Version Number: 1.1.0
==================================================
Added support for setting and getting UNIX time;

  PUT /sensor/arduino/time?1489725078
  GET /sensor/arduino/time?whatever

CoAP resources moved to individual files etc.


Arduino MilliShield Software Version Number: 1.0.0
==================================================
First released version containing temp sensor, humidity sensor, led on/off, 
led blinking fast/slow.  This was the code used in the demo at SSNI in 
San Jose, California on March 1, 2017