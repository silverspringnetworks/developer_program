
### Itron Milli5 Milli-Shield CoAP Reference Package

### Change Log
Please Note: This version is obsolete. It is left for reference only!

#### Version 1.4.5
- Released on Apr 18, 2019
- Fixed the bug where the milli provided date (the epoch) is corrupted when the milli sends a /snsr/sys/time PUT
request to the mshield app. This bug occurs only when mshield is built with the Arduino IDE tool chain.
This bug does not occur when mshield is built using the Atmel Studio tool chain.
Internal bug reference: REFCOAPSERVER-19.


#### Version 1.4.4
- Released on Mar 4, 2019
- Improved the CoAP response if a DHT-xx is not hooked up. The original behavior was to return an internal server error.
The temp sensor code now returns a sensor value of 100C (212F) if a sensor is not hooked up.
The max temperature that can be returned by the DHT-xx family is 80C.
- Added the CoAP Server Reset request.
This is code that sends a PUT request to the milli shield to inform it of an app processor (Adafruit board) reset.
See coap_rbt_msg.cpp for the code.
This file also demonstrates the correct way for a CoAP server to inform the milli of a request or response.
- Added support for setting "system time". The CoAP server now supports the /snsr/sys/time resource.
This is used by the milli to set the app processor time and allows external clients to GET the app processor time.
- Fixed the generic CoAP Server resource /snsr. A GET on this resource now returns the CoAP Server version string.
Note that the version define has been moved from mshield.h to coapsensoruri.h.


#### Version 1.4.1
- Released on Dec 14, 2018.
- Added support for the Adafruit Metro M0 Express board.
This board replaces the Arduino M0 Pro board previously shipped with the HDKs.
Please note that all changes are backward compatible.
That is, mshield 1.4.1 will support both boards.
- Significant code cleanups. Many minor bugs were fixed.
- Fixed several memory leaks.
- Cleaned up and added comments to the code developers look at most.
- The code that supports the reference sensor (the DHT11 temperature sensor) has been significantly improved and commented.
It will be much easier to identify what needs to be changed to add a new sensor.
- Added CBOR encoding library to the code base.
- Added the option to CBOR encode the temperature sensor payloads. See the file temp_sensor.h for details.
This allows for supporting developers who need to work with the upcoming "bubble up" capability.
- Added the ability for logging to be picked up after a serial monitor is closed and then re-opened at a later time.
- Fixed issues around intermittent observe notification generation
- Updated the sensor templates.
- Added ability to turn off CBOR encoding.
- Added a define to set the observation frequency (making it easy).

##### Usage Notes
To turn off CBOR encoding in the temp sensor, locate the TEMP_CBOR_PAYLOAD define located in temp_sensor.h and set it to 0. The default is 1 (CBOR on).
 
```
#define TEMP_CBOR_PAYLOAD    1
#define TEMP_CBOR_PAYLOAD    0
```
 
To set the observation frequency, locate the OBSERVATION_FREQUENCY define located in coapsensorobs.h.
The value is the number of seconds between observation notifications. The default is 60 seconds.
 
```
#define OBSERVATION_FREQUENCY  60            // 1m
#define OBSERVATION_FREQUENCY  3600          // 60m
```
