# Silver Spring Networks - Arduino MilliShied Software

The Milli5-Arduino CoAP Server consists of five sub-folders:

Folder Name | Content
----------- | -----------
mshield | Contains an Arduino sketch that uses the MilliShield libraries
hdlc | The HDLC library
coap_server | The CoAP Server library
coap_rsrcs | Library of CoAP resources; sensors, time, etc.
utils | The Utils library containing logging and buffer management

## Installation Instructions:

1. Download the folder mshield to your local machine and save it at your preferred location e.g.`C:\mshield`

1. Download the remaining four folders to the following location
	e.g.`C:\Users\<user>\Documents\Arduino\libraries`
	**Note**: Replace `<user>` with your username

1. Download and install Arduino IDE 1.8.2
   https://www.arduino.cc/en/Main/Software

1. Click on mshield.ino located in the `mshield` folder

1. In the IDE, go to `Sketch/Include Library/Contributed Libraries`
   1. Select `hdlc`
   1. Repeat Step 5 and select `coap_rsrcs`
   1. Repeat Step 5 and select `utils`
   1. Repeat Step 5 and select `coap_server`

1. In the IDE, go to `Sketch/Include Library/Manage Libraries`
   1. In the search bar, search for RTCZero and install that library
   1. Do you have the DHT11 sensor?
      1. If yes, use `Manage Libraries` to locate and install the following:
			* Adafruit Unified Sensor
			* DHT Sensor Library
