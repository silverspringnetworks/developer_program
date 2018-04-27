# Itron Networked Solutions - Arduino MilliShield Software

**Note: The only version of Arduino IDE supported is for Windows 7, 8 and 10**

This reference application consists of two folders:

Folder Name | Content
----------- | -----------
mshield | The Arduino Sketch dependent on the Coap Server library
ssni_coap_server | The SSNI COAP Server Library

## Installation Instructions:

1. Download the folder `mshield` to your local machine and save it at your preferred location e.g.`C:\mshield`

1. Download the folder `ssni_coap_server` to this location:

	`C:\Users\<user>\Documents\Arduino\libraries`

	**Note**: Replace `<user>` with your username

1. Download and install the Arduino IDE 1.8.2 from one of the below locations depending on your Arduino hardware:
	* https://www.arduino.cc/en/Main/Software
	* http://www.arduino.org/downloads

1. Click on `mshield.ino` located in the `mshield` folder

1. In the IDE, go to `Sketch/Include Library/Contributed Libraries` and select `ssni_coap_server`

1. In the IDE, go to `Sketch/Include Library/Manage Libraries`
   1. In the search bar, search for RTCZero and install that library
   1. Do you have the DHT11 sensor?
      1. If yes, use `Manage Libraries` to locate and install the following:
			* Adafruit Unified Sensor
			* DHT Sensor Library
1. In the IDE, go `Tools/Board/Boards Manager` and the Boards Manager window should appear.
   1. Use the search bar to locate your board e.g. M0 Pro
   1. If your board is an Arduino M0, M0 Pro, Arduino/Genuino Zero or ZERO Pro; use the drop-down menu, select board support package 1.6.14 and click Install
