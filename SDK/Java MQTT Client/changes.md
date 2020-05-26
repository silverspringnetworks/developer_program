
### Java MQTT Client

### Changes

#### Version 1.0.4
- Released on May 26, 2020
- Updated the POM. Minor cleanups and updated dependencies.
- Incorporated suggestions in the README.md file made by Jcrash29.

#### Version 1.0.3
- Released on Aug 1, 2019
- Added a macid attribute to the Starfish Data Platform observations JSON payload for the temp sensor.
This allows the observation consumer to be able to easily associate the payload with a sensor.

#### Version 1.0.2
- Released on Mar 28, 2019
- Fixed bug in the temp sensor payload transformer where the time
was not converted from epoch to timestamp correctly 
Now the SDP timestamp property is formatted correctly.
REFMQTTCLIENT-12.
- Removed the trailing space on the MQTTSDPBRIDGE_BROKER_PROTOCOL
property in the config.properties template.
REFMQTTCLIENT-13.

#### Version 1.0.0
- Released on Feb 25, 2019
- Initial release.
