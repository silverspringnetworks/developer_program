## Itron MIlli5 Sensor API Reference Package

### Current Version Info
Please Note: This version is obsolete. It is left for reference only!
See the changes.md file for details.

### Repository Structure

The Milli Sensor API and CoAP Server consists of two folders:

Folder Name | Content
----------- | -----------
sapi_mshield | The temperature reference application. Uses the Sensor API.
itron_coap_server | The Itron Sensor API and CoAP Server library

### Installation Instructions:

1. From the Milli5-Sensor API repository, copy the sub-folder `sapi_mshield` to `C:\Users\<user>\Documents\Arduino`
**Note:** Replace <user> with your Windows user name.

1. Download the folder `itron_coap_server` to `C:\Users\<user>\Documents\Arduino\libraries`

1. Download and install the Arduino IDE 1.8.10 from https://www.arduino.cc/en/Main/Software

1. In the preferences, locate the field "Additionial Board Manager URLs".
Add this URL to the list: `https://adafruit.github.io/arduino-board-index/package_adafruit_index.json`

1. Using the board manager, add the Adafruit SAMD boards (version 1.5.6). Select the Adafruit Metro M0 Express board.

1. In the folder Arduino\sapi_mshield, click on sapi_mshield.ino and your Arduino IDE will launch

1. In the IDE, go to the Sketch menu and choose Include Library, and select itron_coap_server from the Contributed Libraries section

1. In the IDE, go to Sketch/Include Library/Manage Libraries
1. In the search bar, type RTCZero
 1. Click on "More info"
 1. In the drop-down menu, select v1.6.0 and click Install
1. The following libraries are needed to support the DHT-11 temp sensor (these are needed even if you do not hook a DHT-11 up):
 1. Use Manage Libraries to locate and install the following libraries:
 1. Adafruit Unified Sensor v1.0.2
 1. DHT Sensor Library v1.3.0
1. Build and upload the application to your Adafruit Metreo M0 Express board. Explore and have fun!
