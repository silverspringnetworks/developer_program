# Starfish Studio SDK wrapper with Sensorg connectivity

Snapcraft yaml file to build a snap that will fetch sensor data from TI-Sensortag and post to Startfish Studio platform

# Pre-requisites
a) The machine running the snap must have proper internet connectivity.
b) Latest version (v2.28) of Snapcraft tool must be installed on system.  

# Build the snap
1. Make the snap : ```~$ sudo snapcraft```
 
   When the process completes then you can see "starfish-sensortag-app_0.1_armhf.snap" in the current directory. 

# Install the snap
1. Install the snap : ```~$ sudo snap install starfish-sensortag-app_0.1_armhf.snap --devmode```
2. Attach the bluetooth-control plug : ``~$ sudo snap connect starfish-sensortag-app:bluetooth-control core:bluetooth-control``.  With `~$ snap interfaces` verify the plugs and slots of bluetooth-control for your snap are connected properly.
3. Attach the client plug and service slot of starfish-sensortag-app : `~$ sudo snap connect starfish-sensortag-app:client starfish-sensortag-app:service`

After installing the snap you should be able to see the below snap apps in the `/snap/bin directory`

a) `starfish-sensortag-app.run`

b) `starfish-sensortag-app.hciconfig` 

c) `starfish-sensortag-app.hcitool`

d) `starfish-sensortag-app.bluetoothctl`

# Usage and examples

 To run the snap-app  run the command `starfish-sensortag-app.run` with proper arguments. 
 The usage is as follows : ``` sudo starfish-sensortag-app.run ClientId ClienSecret UUID NoOfObs DelayInObs(ef:5s)``` . 

 The parameters to be passed (in-order) are described below :
 
 1. ClientId : Valid Startfish Studio platform client Id. This is a mandatory parameter.
 2. ClientSecret : Valid Starfish Studio platform client secret. This is a mandatory parameter.
 3. UUID : MAC address of TI-Sensortag obtained from lescan. This is a mandatory parameter.
 4. NoOfObs : Number of observations to be fetched from the TI-sensortag. This is a mandatory parameter.
 5. DelayInObs : The delay to be observed between fetching two readings. This parameter is an optional and the default value is 5 seconds.
   
  # Obtaining UUID of TI-Sensortag
 
    1. First ensure you have a bluetooth controller up and running with `starfish-sensortag-app.hciconfig`.

    2. Now use `starfish-sensortag-app.hcitool lescan` to scan for TI-Sensortag.

        ```
        ~$ starfish-sensortag-app.hcitool lescan
        LE Scan ...
        A0:E6:F8:B6:E7:86 (unknown)
        A0:E6:F8:B6:E7:86 CC2650 SensorTag
        ```
 

This snap works with the TI CC2650 multi-standard SensorTag and Starfish SDK. It obtains the following readings from sensors and posts them to Starfish Data Platform: 

- Temp: Ambient and Object temperature (Cs)
- Humidity: Ambient temperature (Cs) and relative humidity (%)
- Barometer: Temperature (Cs) and Air Pressure (hPa)
- Accelerometer: X,Y,Z axis with a default period for the data is one second. (Gs)
- Magnetometer: Strength of a magnetic field in the three axis (uT)
- Gyroscope: Rotational motion (degrees/second)
- Light: Ambient Light (Lux)

    
# Format of obsevations posted to the Starfish Studio

"Temp" : "26.125, 19.375"

"Humidity" : "26.31622314453125, 48.590087890625"

"Barometer" : "26.55, 1018.32"

"Accelerometer" : "0.049560546875, 0.060791015625, -0.94140625"

"Magnetometer" : "-20.841514041514042, 21.44126984126984, -31.337240537240536"

"Gyroscope" : "1.13677978515625, 0.091552734375, -2.54058837890625"

"Light" : "7.640000000000001"

"Battery" : "84"

