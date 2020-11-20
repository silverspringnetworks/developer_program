### Milli5 Sensor API

### Change Log

#### Version 1.0.2 (CoAP Server Version 1.4.8)
- Released on Nov 19, 2020
- This version introduces the MCU portability layer.
This simplifies the job of moving SAPI and the Itron CoAP Server to a new MCU.
All MCU specific code has been isolated in these files:
    - portability.h
    - portability.cpp
    - arduino_time.h
    - arduino_time.cpp
- Changed the error type from error_t to error_cs_t to avoid compilation issues with some manufacturers CMSIS core.
- Removed the need for sapi_mshield.h. Moved forward declarations to sapi.h. Deleted sapi_mshield.h.
- Moved Serial_ declaration to the portability layer and added STM32 HardwareSerial support.
Updated the logging code so it is not dependent on Serial_.
This removes a porting issue and males the STM32 board support easier.
- Changed CoAP response payload content type from COAP_CF_CSV to COAP_CF_APPLICATION_CBOR
- In EchoSensor.cpp added a uint8_t typecast when setting the response length.
Removes a compile warning in the Arduino IDE.
- Removed sapi_error include in Sketch drivers. It is redundant and not needed.
- Updated the copyright header to reflect Itron branding.
- Updated code removing all references to "milli" replacing with "Itron NIC".
- Added the portability support file (portability.h). Deleted arduino_pins.h.
- Added the portability support file (portability.cpp).
- Refactored platform specific RTC code.
The arduino_time files are now part of the portability support layer and isolate the SAPI from RTC MCU specific code.
- Minor code and comment cleanup in the CoAP Server.
- Increased the time the Milli wake pin is held low from 1 to 2 milliseconds when pushing observation responses (notifications).
- Added the ability to push notifications without generating periodic observations.
On the  sapi_register_sensor  call set the  is_observer  parameter to 1 and the  frequency  parameter to 0.
- Fixed the bug where the classifier (the middle element of a CoAP sensor URI) specified in the  sapi_initialize
call is ignored by the code the set up the resource observer table.
Now the classifier properly works when generating observation responses (notifications).
- Extended the maximum length of the classifier and device type (the leaf element of a CoAP sensor URI) to 16 characters each
(which includes the "C" end of string (0x00) value).
However, there are length limitations imposed by the MIlli sysvar 35 setting. See the discussion below.

#### Version 1.0.0 (CoAP Server Version 1.4.6)
- Released on Oct 30, 2019
- The initial release of Sensor API. Sensor API hides all the CoAP complexity and makes adding Sensors easy.
You can spend your time building sensor code and not CoAP!
See the SAPI documentation, and the developer portal for details. 
