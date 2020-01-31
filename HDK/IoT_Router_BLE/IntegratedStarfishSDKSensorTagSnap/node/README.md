# Starfish Studio SDK wrapper with Sensor connectivity

Snapcraft yaml file to build a snap that will fetch sensor data from the TI CC2650 Sensortag and post it to the Itron Starfish Studio platform

# Pre-requisites
a) The machine running the snap must have proper internet connectivity.
b) Latest version (2.35) of the Snapcraft tool must be installed on the system.  

# Build the snap
1. Make the snap : ```~$ snapcraft```
 
When the process successfully completes you will be able to see "starfish-sensortag-app_0.4_armhf.snap" in the current directory. 

# Install the snap
1. Install the snap : ```~$ sudo snap install --dangerous starfish-sensortag-app_0.4_armhf.snap```
2. Attach the bluetooth-control plug : ```~$ sudo snap connect starfish-sensortag-app:bluetooth-control core:bluetooth-control```.
3. Attach the client plug and service slot of starfish-sensortag-app : ```~$ sudo snap connect starfish-sensortag-app:client starfish-sensortag-app:service```
4. With ```~$ snap interfaces``` verify the plugs and slots of the snap are connected properly.

After installing the snap you should be able to see the below snap apps in the ```/snap/bin directory```

a) ```starfish-sensortag-app.hciconfig```
b) ```starfish-sensortag-app.hcitool```
c) ```starfish-sensortag-app.run```
d) ```starfish-sensortag-app.sensortag```

# Usage and examples

To run the snap-app run the command ```starfish-sensortag-app.run``` with proper arguments. 
The usage is as follows : ``` sudo starfish-sensortag-app.run ClientId ClientSecret MAC NoOfObservations Delay```. 

The parameters to be passed (in-order) are described below:

1. ClientId : Valid Startfish Studio platform client Id. This is a mandatory parameter.
2. ClientSecret : Valid Starfish Studio platform client secret. This is a mandatory parameter.
3. MAC/UUID : MAC address of the TI Sensortag obtained from lescan. This is a mandatory parameter.
4. NoOfObservations : Number of observations to be fetched from the TI Sensortag. This is a mandatory parameter.
5. Delay : The delay to be observed between readings. Set to 0 for a single observation.

# Obtaining MAC/UUID of TI Sensortag

1. First ensure you have a bluetooth controller up and running with ```starfish-sensortag-app.hciconfig```.
2. Now use ```sudo hcitool lescan``` to scan for TI-Sensortag.

	```
	~$ sudo hcitool lescan
	LE Scan ...
	54:6C:0E:52:F4:5A (unknown)
	54:6C:0E:52:F4:5A CC2650 SensorTag
        ```

This snap works with the TI CC2650 multi-standard SensorTag and Starfish SDK. It obtains the following readings from sensors and posts them to the Starfish Data Platform: 

- Temp: Ambient and Object temperature (C). Zero in later version of the SensorTag
- Humidity: Ambient temperature (C) and relative humidity (%)
- Barometer: Temperature (C) and Air Pressure (hPa)
- Accelerometer: X,Y,Z axis with a default period for the data is one second. (Gs)
- Magnetometer: Strength of a magnetic field in the three axis (uT)
- Gyroscope: Rotational motion (degrees/second)
- Light: Ambient Light (Lux)
- Battery (%)

    
# Format of obsevations posted to the Starfish Studio

Temp:  (0.0, 0.0)
Humidity:  (24.69482421875, 72.65625)
Barometer:  (24.69, 1006.63)
Accelerometer:  (0.0703125, 0.04052734375, 1.99951171875)
Magnetometer:  (-9.895970695970696, 25.18974358974359, -33.43638583638584)
Gyroscope:  (-2.777099609375, 1.495361328125, 0.8697509765625)
Light:  422.08
Battery:  82

