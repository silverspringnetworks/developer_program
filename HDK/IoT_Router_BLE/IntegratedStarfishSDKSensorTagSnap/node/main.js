#!/usr/bin/env node

// 01 Nov 2019: Iain Mackenzie, iain.mackenzie@itron.com

// includes
//const chalk = require('chalk');
//const clear = require('clear');
const starfishSdk = require('starfish-sdk');
const spawnProcess = require('child_process').spawn;


const getDevice = async (starfish, macAddress) => {
    return new Promise((resolve, reject) => {
        starfish.getDevices((error, response) => {
            if (error) {
                reject(error);
            } else {
                for (i = 0; i < response.length; i++) {
                    if (response[i].domainInfo.macaddress === macAddress) {
                        deviceId = response[i].id;
                        console.log('\nDevice found. Id: ' + deviceId);
                        resolve(deviceId);
                    }    
                }    
                
                resolve(undefined);
            }
        });        
        
    });
};

const createDevice = async(starfish, macAddress) => {
    return new Promise((resolve, reject) => {
        const testDevice = {
            "deviceType": "SensorTag",
            "domainInfo": {
              "modelName": "CC2650",
              "macaddress": macAddress,
              "timestamp": new Date().toISOString()
            }
        };

        starfish.postDevice(testDevice, (error, response) => {
            if (error) {
                reject(error);
            } else {
                deviceId = response.id;
                console.log('\nDevice was successfully created. Id: ' + deviceId);
                resolve(deviceId);
            }
        });

    });
};

const uploadDeviceObservation = async (starfish, deviceId, sensor_obs) => {
    return new Promise((resolve, reject) => {
        starfish.postDeviceObservation(deviceId, sensor_obs, (error, response) => {
            if (error) {
                reject(error);
            } else {
                //console.log(response)
                resolve(true);
            }
        });
    });
};

const getObservation = async (starfish, macAddress, deviceId, noOfObs, delay) => {
    return new Promise((resolve, reject) => {
        console.log('Taking a measurement...')

        var receivedData = '';
        
        var environment = process.env;
        environment.PYTHONUNBUFFERED = 1;   // Same as python3 -u option        
    
        //use a child process for obtaining observations from the TI device           
        const child = spawnProcess('sensortag', ['-n ' + noOfObs,'-t ' + delay,'-T','-H','-B','-A','-M','-G','-L','-P', 
            macAddress],{shell: true, env: environment});
    
        child.stdout.on('data', async (chunk) => {
            try {
                receivedData += chunk.toString();
                //console.log(chunk.toString());
                if (receivedData.search('Temp') != -1 && receivedData.search('Battery') != -1) {
                    var observation = receivedData;
                    receivedData = '';
        
                    console.log('\n\nOutput:\n' + observation);
        
                    observation = observation.slice(observation.search('Temp'), observation.length);
                    observation = observation.replace(/\n/g,","); // newline to ,
                    observation = observation.replace(/[(a-zA-Z( ):\t]/g,""); // remove all non numeric
                    observation = observation.replace(/'',/g,""); // remove empty fields
                    var sdata = observation.split(",");
        
                    if (sdata.length == 18) {
                        // add timestamp
                        var timestamp = new Date().toISOString();
                        
                        var accelerometer = {"x": parseFloat(sdata[ 6]), "y": parseFloat(sdata[ 7]), "z": parseFloat(sdata[ 8])}; // G
                        var magnometer    = {"x": parseFloat(sdata[ 9]), "y": parseFloat(sdata[10]), "z": parseFloat(sdata[11])}; // uT
                        var gyroscope     = {"x": parseFloat(sdata[12]), "y": parseFloat(sdata[13]), "z": parseFloat(sdata[14])}; // degrees/S
        
                        var sensor_obs = {
                            "observations": [{
                                "timestamp": timestamp,
                                "temperature": parseFloat(sdata[0]),  // C
                                "humidity": parseFloat(sdata[3]),     // %RH
                                "barometer": parseFloat(sdata[5]),    // hPa
                                "accelerometer": accelerometer,       // G
                                "magnetometer": magnometer,           // uT
                                "gyroscope": gyroscope,               // degrees/S
                                "light": parseFloat(sdata[15]),       // LUX
                                "percentlevel": parseFloat(sdata[16]) // %charge
                            }]
                        };

                        await uploadDeviceObservation(starfish, deviceId, sensor_obs);
                        
                    } else {
                        console.log("No or corrupted sensor data\n");
                    }
                }    
            } catch (error) {
                reject(error);    
            }
        });
    
        child.stderr.on('data', (chunk) => {            
            console.log(chunk.toString());            
            reject(new Error(''));            
        });

        child.on('error', (error) => {            
            reject(error);
        });

        child.on('exit', (code, signal) => {
            //console.log(`Exit code is:  ${code} and signal: ${signal}`);
            if (!isNaN(code) && code == 0 && signal == null) {                
                resolve(true);
            }
            
        });
    });

};

function checkClientId(value) {
    try {
        const pattern = /\b[0-9abcdef]{8}-[0-9abcdef]{4}-[0-9abcdef]{4}-[0-9abcdef]{4}-[0-9abcdef]{12}\b/i
        return pattern.test(value.trim());
    } catch (error) {
        return false;        
    }
}

function checkMACAddress(value) {
    try {       
        // Example: 54:6C:0E:52:F4:5A
        
        const pattern = /\b[0-9ABCDEF]{2}:[0-9ABCDEF]{2}:[0-9ABCDEF]{2}:[0-9ABCDEF]{2}:[0-9ABCDEF]{2}:[0-9ABCDEF]{2}\b/i
        return pattern.test(value.trim());
    } catch (error) {
        return false;        
    }
}

const run = async () => {
    try {        
        if (process.argv.length != 7) {        
           console.log('Usage : ' + process.argv[0] + ' ' + process.argv[1] 
            + ' <ClientId> <ClientSecret> <MAC> <NoOfObservations> <Delay>');
           process.exit(-1);
        }
        
        // Check the ClientId
        const clientId = process.argv[2].trim().toLowerCase();
        if (!checkClientId(clientId)) {
            console.log('The value entered for the ClientId is invalid')
            process.exit(-1);    
        }

        // Check the MAC    
        const macAddress = process.argv[4].trim().toUpperCase();
        if (!checkMACAddress(macAddress)) {
            console.log('The value entered for the MAC is invalid')
            process.exit(-1);    
        }

        // Check the NoOfObservations and Delay
        var noOfObs = process.argv[5];
        var delay = process.argv[6];
        if ((isNaN(noOfObs)) || (isNaN(delay))) {
            console.log('The values entered for the NoOfObservations or Delay are invalid')
            process.exit(-1);
        } else {
            noOfObs = Math.round(noOfObs);
            delay = Math.round(delay);
            
            if ((noOfObs < 1) || (noOfObs > 60)) {
                console.log('The value entered for the NoOfObservations is invalid')
                process.exit(-1);    
            }

            if (noOfObs != 1) {
                if ((delay < 2) || (delay > 60) || (noOfObs * delay > 120)) {
                    console.log('The values entered for the NoOfObs or Delay are invalid')
                    process.exit(-1); 
                }
            }
        }        
        
        // Create an object of StarfishService to communicate with the SSN Starfish data platform
        const options = {
            'credentials' : {
            'clientId' : clientId,
            'clientSecret' : process.argv[3].trim()
            },
            'solution' : 'sandbox'
        };
    
        const starfish = new starfishSdk(options);
    
        var deviceId = await getDevice(starfish, macAddress);
        if (deviceId == undefined) {
            deviceId = await createDevice(starfish, macAddress);    
        }

        await getObservation(starfish, macAddress, deviceId, noOfObs, delay);        
                
    } catch (error) {
        console.log(error);    
    }
};

// Clear the console
//clear();

// run the program
run();