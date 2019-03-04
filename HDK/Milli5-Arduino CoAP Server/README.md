## Itron Arduino Milli-Shield

### Current Version Info
Current version is 1.4.4.
See the changes.md file for details.

### Repository Structure

The Milli Shield CoAP Server consists of two folders:

Folder Name | Content
----------- | -----------
mshield | The Arduino Sketch dependent on the CoAP Server library
ssni_coap_server | The Itron CoAP Server Library

### Installation Instructions:

1. From the Milli5-Arduino CoAP Server repository, copy the sub-folder `mshield` to `C:\Users\<user>\Documents\Arduino`
**Note:** Replace <user> with your Windows user name.

1. Download the folder `ssni_coap_server` to `C:\Users\<user>\Documents\Arduino\libraries`

1. Download and install the Arduino IDE 1.8.7 from https://www.arduino.cc/en/Main/Software

1. In the preferences, locate the field "Additionial Board Manager URLs".
Add this URL to the list: `https://adafruit.github.io/arduino-board-index/package_adafruit_index.json`

1. Using the board manager, add the Adafruit SAMD boards. Select the Adafruit Metro M0 Express board.

1. In the folder Arduino\mshield, click on mshield.ino and your Arduino IDE will launch

1. In the IDE, go to the Sketch menu and choose Include Library, and select ssni_coap_server from the Contributed Libraries section

1. In the IDE, go to Sketch/Include Library/Manage Libraries
1. In the search bar, type RTCZero
 1. Click on "More info"
 1. In the drop-down menu, select v1.5.2 and click Install
1. The following libraries are needed to support the DHT-11 temp sensor (these are needed even if you do not hook a DHT-11 up):
 1. Use Manage Libraries to locate and install the following libraries:
 1. Adafruit Unified Sensor v1.0.2
 1. DHT Sensor Library v1.3.0
1. Build and upload the application to your Adafruit Metreo M0 Express board. Explore and have fun!
