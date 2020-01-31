#!/usr/bin/env node

// 01 Nov 2019: Iain Mackenzie, iain.mackenzie@itron.com

// includes
//const chalk = require('chalk');
//const clear = require('clear');
const starfishSdk = require('starfish-sdk');
const util = require('util');
const execProcess = util.promisify(require('child_process').exec);


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
            "deviceType": "RubishBin",
            "domainInfo": {
              "modelName": "Sensoneo Quatro",
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

const getObservation = async (starfish, macAddress, deviceId) => {
    console.log('Taking a measurement...')
    //use a child process for obtaining observations from the Sensoneo device   
    const { stdout, stderr } = await execProcess('sensoneo -n 1 ' + macAddress);
    if (stderr) {
        console.log('\n\nError:\n' + stderr);
        return false;
    }

    // read results    
    var observation = stdout;
  
    console.log('\n\nOutput:\n' + observation);

    if (observation.search('Distances') != -1 && observation.search('Tilt') != -1) {
        observation = observation.slice(observation.search('Distances'), observation.length); //skip second line
        observation = observation.replace(/\n/g,","); // newline to ,  
        observation = observation.replace(/[(a-zA-Z( ):\t]/g,""); // remove all non numeric
        observation = observation.replace(/'',/g,""); // remove empty fields
        var sdata = observation.split(",");

        if (sdata.length >= 7) {
            var distances = {"1": parseFloat(sdata[0]), "2": parseFloat(sdata[1]), "3": parseFloat(sdata[2]), "4": parseFloat(sdata[3])};    
    
            var sensor_obs = {
                "observations": [{
                    "timestamp": new Date().toISOString(),
                    "distances": distances,
                    "tilt": parseFloat(sdata[6]),
                    "temperature": parseFloat(sdata[5]),
                    "battery": parseFloat(sdata[4])
                }]
            };
     
            try {
                return await uploadDeviceObservation(starfish, deviceId, sensor_obs);                   
            } catch (error) {
                console.log(error);
                reject(new Error(''));    
            }
            
        } else {
            console.log("Corrupted sensor data length of " + sdata.length);                    
        }
    } else {
        console.log("No or corrupted sensor data\n");
    }
    
    return false;
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
        if (process.argv.length != 5) {        
            console.log('Usage : ' + process.argv[0] + ' ' + process.argv[1] 
                + ' <ClientId> <ClientSecret> <MAC>');
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
            console.log('The value entered for the BLE MAC is invalid')
            process.exit(-1);    
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

        await getObservation(starfish, macAddress, deviceId);
        
                
    } catch (error) {
        console.log(error);    
    }
};

// Clear the console
//clear();

// run the program
run();