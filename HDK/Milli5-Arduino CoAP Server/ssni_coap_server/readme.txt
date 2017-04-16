==================================================
|        Arduino MilliShield Software            |
==================================================

This deliverable consists of two folders:

1. mshield 		     - The Arduino sketch that depends on the CoAP Server library
2. ssni_coap_server  - The CoAP Server library

Software Installation Instructions:
-----------------------------------

1. From the following GitHub location, download the Arduino MilliShield software:

https://github.com/silverspringnetworks/developer_program/tree/master/HDK/Milli5-Arduino%20CoAP%20Server

2. Download the folder mshield to anywhere on your computer system e.g. c:\mshield

3. Download the folder ssni_coap_server to the following location
   NOTE: replace <user> with your username

		C:\Users\<user>\Documents\Arduino\libraries

4. Download and install Arduino IDE 1.8.2; 
   https://www.arduino.cc/en/Main/Software or http://www.arduino.org/downloads 
   Download from the company associated with your hardware

5. Click on mshield.ino located in the folder mshield and the Arduino IDE will launch

6. In the IDE, go Sketch/Include Library/Contributed Libraries and select ssni_coap_server

7. In the IDE, go Sketch/Include Library/Manage Libraries;

	a) In the search bar, search for RTCZero (or RTCDue) and install that library
	b) Do you have the DHT11 sensor?
		i) If yes, use "Manage Libraries" to locate and install the following:
			1. Adafruit Unified Sensor
			2. DHT Sensor Library

More info:

https://zoo.silverspringnet.com/display/platform/Set-up+and+Install
https://portal7-stage.mitangi.com/milli-developer-kit-arduino-v10-getting-started-guide
https://portal7-stage.mitangi.com/milli-developer-kit-reference-application

+++++++++++++++++
+ Release Notes +
+++++++++++++++++


Arduino MilliShield Software Version Number: 1.3.0
==================================================
- Added support for Observe on a sensor defined by an end-user
- Added support for adding a customer defined sensor
  A template is provided for speeding up the process of adding a sensor
- Added support for turning on or off the logging
- Consolidated the library code into one library called ssni_coap_server
  This will simplify the installation process
- Remove all sensors except for the 4-pin DHT11 sensor
  The size after removing sensors is 49 KBytes
- In mshield.ino and mshield.h, there is a #define called SMALL_FOOTPRINT
  This define can be used to switch between the single sensor case and the
  multi-sensor case. In SVN, by default, SMALL_FOOTPRINT is un-defined.
- Got rid of the need for Tera Term and replaced it with the build-in
  Serial Monitor using the Native Port
- Added more Doxygen code to explain interfaces, usage etc to the end-user
- Added more content to the template used for adding new sensors
  This will help the end-user create "observable" sensors
- Changed the naming of the pointers to the serial ports to make it clearer
- Tested the code on Arduino M0 Pro


Arduino MilliShield Software Version Number: 1.2.5
==================================================
- Added support for Arduino SAM architecture. This is for RTCDue as needed by Arduino Due.
- Remove a lot of header files that were not needed, especially in the CoAP source code
- Remove the dependency on mshield.h in the libraries
- Pass pointer to Serial object to the Logging library
  Functions that need the Serial object would need to obtain it for Logging 
  using the new method provided; log_get_serial()
  The file log.h defines Serial and this file should be included whereever you
  need Serial with the exception of the .ino file.  The .ino file will contain
  the "root" definition of Serial in the header file mshield.h.
- Adding a function for printing a buffer
- Added methods for propagating a pointer to the UART serial to the HDLC function
  Init of UART serial is now an HDLC function. This pointer is set in mshield.h.
- Re-write of the LED resource by adding a config function with defines in mshield.h
- Make the LED behavior configurable in mshield.h
- Make the Time Zone configurable in mshield.h
- Make the Max-Age configurable in mshield.h
- Make the UART time-out period configurable in mshield.h
- Make the console serial baud rate configurable in mshield.h
- Tested the code on Arduino M0 Pro and Arduino Zero and works on both
  On Arduino Zero, you need to restart Tera Term after reset or uploading


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