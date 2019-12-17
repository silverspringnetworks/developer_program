# Starfish Studio SDK wrapper with Sensor connectivity

Snapcraft yaml file to build a snap that will fetch sensor data from the Sensoneo Single or Quatro sensor and post it to the Starfish Studio platform

# Pre-requisites
a) The machine running the snap must have proper internet connectivity.
b) Latest version (2.35) of the Snapcraft tool must be installed on the system.  

# Build the snap
1. Make the snap : ```~$ snapcraft```

When the process successfully completes you will be able to see "starfish-sensoneo-app_0.1_armhf.snap" in the current directory.

# Install the snap
1. Install the snap : ```~$ sudo snap install --dangerous starfish-sensoneo-app_0.1_armhf.snap```
2. Attach the bluetooth-control plug : ```~$ sudo snap connect starfish-sensoneo-app:bluetooth-control core:bluetooth-control```
3. Attach the client plug and service slot of starfish-sensoneo-app : ```~$ sudo snap connect starfish-sensoneo-app:client starfish-sensoneo-app:service```
4. With ```~$ snap interfaces``` verify the plugs and slots of the snap are connected properly.

After installing the snap you should be able to see the below snap apps in the ```/snap/bin directory```

a) ```starfish-sensoneo-app.run```
b) ```starfish-sensoneo-app.blescan``` 

# Usage and examples

To run the snap-app run the command ```starfish-sensoneo-app.run``` with proper arguments. 
The usage is as follows : ```sudo starfish-sensoneo-app.run ClientId ClientSecret MAC```. 

The parameters to be passed (in-order) are described below:

1. ClientId : Valid Starfish Studio platform client Id. This is a mandatory parameter.
2. ClientSecret : Valid Starfish Studio platform client secret. This is a mandatory parameter.
3. MAC : MAC address of the Sensoneo Single or Quatro sensor obtained from blescan. This is a mandatory parameter.
   
# Obtaining MAC of the Sensoneo Single or Quatro sensor

1. First ensure you have a bluetooth controller up and running.
2. Now use ```sudo starfish-sensoneo-app.blescan``` to scan for the Sensoneo Single or Quatro sensor.

	```
	~$ sudo starfish-sensoneo-app.blescan
    Device (new): d4:ca:6e:b6:44:e3 (public), -62 dBm
        Flags: <06>
        Complete Local Name: 'QS02100007'
        Manufacturer: <710004635e5e5b75193febaaae5d000000004e0000000045cd82>
        ```

This snap works with the Sensoneo Single or Quatro sensor and Starfish SDK. It obtains the following readings from sensors and posts them to Starfish Data Platform: 

- Distances: Distances from the four rubish sensors (cm)
- Battery: Voltage (V)
- Temperature: Temperature (C)
- Tilt: (Degrees)
