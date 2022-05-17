## CMSIS based Itron Sensor API reference package for the Milli5 and Milli Shield

### Current Version Info
Current Sensor API version is 1.0.2.
Current CoAP Server version is 1.4.8.
See the changes.md file for details.

### Repository Structure
The CMSIS Sensor API reference package consists of the folders:

Folder Name | Content
----------- | -----------
adafruit | The Adafruit specifc core files
CMSIS | CMSIS General ARM support files
CMSIS-Atmel | CMSIS ARM specific files for Atmel produced ARM MCU'sapi_mshield
Libraries | Open source library source code
SensorAPI | The Itron Sensor API and CoAP Server source code
Sensors | Sensor code for the sensors supported by the referenece package

### Installation Instructions:
The Sensor API reference package uses CMSIS open source support files, libraries, and ARM MCU abstraction files.
It is designed to be a stand-alone package that can be built with CMake based tool chains.
The code and documentation has been built and tested using the JetBrains CLion IDE.
Itron supports the use of the CLion IDE in its Partner Developer Program.

To build and install the CMSIS based Sensor API package please refer to the instructions located
on the Itron Developer Program Portal at: https://developer.itron.com/content/building-sensor-api-reference-application-clion
