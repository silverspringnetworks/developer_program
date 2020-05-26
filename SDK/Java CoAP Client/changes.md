
### Java CoAP Client

### Changes

#### Version 1.4.3
- Released on May 26, 2020
- Fixed issues in the POM that might affect future build stability.
- Fixed issue where CoAP requests are always sent as CON.
The --non option properly generates a NON request.
- Removed all support for the defunct Starfish Test network.

#### Version 1.4.2
- Released on Mar 4, 2019
- Fixed bug in the temp sensor payload transformer (internal reference REFCOAPCLIENT-12).
The unix epoch provided in the sensor payload was not converted correctly
causing the timestamp value generated in the Starfish Data Platform observations
JSON to be incorrect.
- Added a Release directory to the repo.
The latest one-jar from the build will be placed here.

#### Version 1.4.1
- Released on Dec 14, 2018.
- Added support for the Adafruit Metro M0 Express board.
- Added support for the New Cosmos methane sensor.
- Added support for the Itron pole tilt sensor.
- Add support for CBOR formated payloads form temp, pole tilt, and methane sensors.
- Fixes the exceptions that occur when a sensor GET returns an error or the payload
is not recognized.
- Many bug and stability fixes.
