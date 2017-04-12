#!/usr/bin/env node
var StarfishService = require('./starfish.js');

/* Checking if correct number of arguments are passes */
if ((process.argv.length  < 6) || (process.argv.length > 7))
{
   console.log("Usage : " + __filename + " [ClientId] [ClienSecret] [UUID] [NoOfObs] [DelayInObs(5s)]");
   process.exit(-1);
}

var filename = process.env.SNAP_COMMON + '/data.txt'
var exceptionFilename = process.env.SNAP_COMMON + '/exception.txt'

//Spawning a child process for checking TI-Sensortag device UUID*/
 const spawnSync = require('child_process').spawnSync;
 const ls = spawnSync('sensortag', ['--all', '-n 1','-f ',filename, process.argv[4]]);
 var out = ls.stderr.toString();
 if (out.includes('Failed to connect')) {
     console.log("Error!! Invalid SensorTag UUID");
     process.exit(-1);
 }

var delayInObs=process.argv[6]
if (delayInObs == undefined) {
  delayInObs = 5;
}

// Creating an object of StarfishService to communicate with the Starfish Studio platform
const options = {
        'endpoint' : 'https://poc.api.ssniot.cloud',
        'credentials' : {
         'clientId' : process.argv[2],
         'clientSecret' : process.argv[3]
        },
       'solution' : 'sandbox'
     };

var starfish = new StarfishService(options)
var deviceId = 0;  //Variable to store the device ID generated for the device to be registered
const testDevice = {"deviceType": "SensorTag", "domainInfo": {"modelName": "CC2650", "macaddress": process.argv[4]}} //Variable to store the info for the device to be registered

 //Registering a device and obtaining the device ID if registered successfully else prompt error message and exit
starfish.postDevice(testDevice, (err, response) => {
 if (err) {
     console.log("Error!!postDevice: " + err);
     process.exit(-1);
   } else {
       deviceId = response.id;
       console.log("Device is Created Successfully: DeviceId: " + deviceId);
       //Spawning a child process for obtaining observations from the TI-Sensortag device
       const spawn = require('child_process').spawn;
       const lsNew = spawn('sensortagUpdate', [process.argv[5], delayInObs, process.argv[4], filename , exceptionFilename]);
   }
  });

//Creating a file event notifier to read observations from a file and post it to the Starfish Studio platform
var fs = require('fs');
var Inotify = require('inotify').Inotify;
var inotify = new Inotify();
var dataUpdateCount = 0;
var watcher = {
 path: process.env.SNAP_COMMON + '/',
 watch_for: Inotify.IN_CLOSE_WRITE,
 callback: function (event) {
 var file = event.name ? event.name : '';
 var mask = event.mask;

 if ((file == 'data.txt') && (file.indexOf('.') !== 0) && (deviceId != 0)) {
   if (mask & Inotify.IN_CLOSE_WRITE) {

      var timestamp = new Date().toISOString();
      console.log("\n\n\nNotify Event TimeStamp " + timestamp + "\n");
      var file = fs.readFileSync(filename, "utf8" );
      var observation = file;
      var closing_session = file;
      observation = observation.replace(/\n/g, ",");
      observation = observation.replace(/[(a-zA-Z( ):]/g,"");
      var sensor_data = observation.split(",");
      if (sensor_data.length == 18) {
     	  var temp = sensor_data[0] + ', ' + sensor_data[1];
    	  var humidity = sensor_data[2] + ', ' + sensor_data[3];
    	  var barometer = sensor_data[4] + ', ' + sensor_data[5];
    	  var accelerometer = sensor_data[6] + ', ' + sensor_data[7] + ', ' + sensor_data[8];
    	  var magnometer = sensor_data[9] + ', ' + sensor_data[10] + ', ' + sensor_data[11];
    	  var gyroscope = sensor_data[12] + ', ' + sensor_data[13] + ', ' + sensor_data[14];

     	  var sensor_obs = { "observations": [{"timestamp": new Date().toISOString(),"temperature": sensor_data[0], "Full temperature": temp, "Humidity": humidity, "Barometer": barometer ,
                                               "Accelerometer": accelerometer, "Magnetometer": magnometer, "Gyroscope": gyroscope, "Light": sensor_data[15], "Battery": sensor_data[16]}] };

  	  starfish.postDeviceObservation(deviceId, sensor_obs, (err, response) => {
   	  if (err) {
      	      console.log("Error!!postDeviceObservation: ! " + err);
          } else {
    	      console.log("Observation is Posted Successfully for DeviceId: " + deviceId);
            }
  	  });

   	  dataUpdateCount += 1;
          console.log("Data is posted successfully " + dataUpdateCount + " times\n");
      } else {
          if (closing_session == "Closing Session") {
            console.log("Final Count: Data is posted successfully " + dataUpdateCount + " times");
            inotify.close();
          }
          else {
          console.log("Corrupted Sensor Data : " + sensor_data);
          console.log("Corrupted Sensor Data Length : " + sensor_data.length);
        }  
        }
   }
 }
 }
};
inotify.addWatch(watcher);
