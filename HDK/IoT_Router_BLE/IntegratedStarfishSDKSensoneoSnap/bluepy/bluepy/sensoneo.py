from bluepy.btle import Scanner, UUID, Peripheral, Service, Descriptor, DefaultDelegate, AssignedNumbers, BTLEDisconnectError, BTLEInternalError
import struct
#import math
import re

def _SENSONEO_UUID(val):
    return UUID('2456e1b9-26e2-8f83-e744-%12X' % (0xF00000000000+val))

class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)

    def handleDiscovery(self, dev, isNewDev, isNewData):
        if isNewDev:
            if (dev.addrType == 'public') and (dev.connectable):            
                print('Discovered BLE device %s (%s), RSSI=%ddBm' % (dev.addr, dev.addrType, dev.rssi))
        elif isNewData:
            pass
            #print('Received new data from', dev.addr)
        else:
            if (dev.addrType == 'public') and (dev.connectable):
                print('Update device %s (%s), RSSI=%ddBm, Update Count=%d' % (dev.addr, dev.addrType, dev.rssi, dev.updateCount))


class SerialBase:
    serviceUUID = None
    dataUUID = None

    def __init__(self, periph):
        self.periph = periph    # really rubish_bin
        self.service = self.periph.getServiceByUUID(self.serviceUUID)        
        self.data = self.service.getCharacteristics(self.dataUUID)[0]  # 0 required as a list is returned

    def enable(self):
        raise NotImplementedError   # abstract class

    def disable(self):
        raise NotImplementedError

class Serial(SerialBase):
    serviceUUID = UUID('2456e1b9-26e2-8f83-e744-f34f01e9d701')
    dataUUID = UUID('2456e1b9-26e2-8f83-e744-f34f01e9d703') # data or fifo characteristic
    creditUUID = UUID('2456e1b9-26e2-8f83-e744-f34f01e9d704') # credit characteristic

    def __init__(self, periph):
        SerialBase.__init__(self, periph)

    def enable(self):   # of notifications        
        result = False
        self.char_descr = self.service.getDescriptors(forUUID=0x2902)[0]    # data descriptor
        self.char_descr.write(struct.pack('<bb', 0x01, 0x00), True)
        return not result

    def disable(self):  # of notifications
        self.char_descr.write(struct.pack('<bb', 0x00, 0x00), True)

    def write(self, val):
        self.data.write(val, True)

    def decode_response(self):
        try:
            return self.periph.delegate.last_value.decode('utf-8','strict')
        except UnicodeError:
            return ''
        except:
            return ''        

    def read(self):
        response = ''
        counter = 0
        while True:
            if (self.periph.waitForNotifications(5.0)):
                response += self.decode_response()
                #print(response)
                if '\r' in response:
                    break
            else:   # timeout
                counter += 1                
                if counter >= 3:
                    print('Timeout')
                    break

        return response        

    def get_pin(self, id):        
        pin = str(int(id[-4:], 16)) # last 4 chars
        return b''.join([bytes(pin ,'utf-8'),bytes.fromhex('0d')])

    def send_pin(self):  # connect and send the pin        
        result = False
        self.write(b'a\r')        
        response = self.read()        
        if 'ENTER PIN' in response:            
            self.write(self.get_pin(self.periph.sensor_id))  # Pin 7
            response = self.read()
            if 'PIN OK' in response:
                result = True
            
            if (response != ''):
                print(response)       

        return result

    def keep_alive(self):
        #self.write(b'x\r')
        self.write(b''.join([b'GetDiagnostic',bytes.fromhex('0d')]))
        return self.read()        

    def parse_measurement(self, val):
        result = ''
        if 'MEAS' in val:
            pattern = re.compile('[0-9]+')
            m = re.findall(pattern, val)
            if m.__len__() == 7:            
                result = 'Distances:  (' + format(int(m[0]),'.1f') + ', ' + format(int(m[1]),'.1f') + ', ' + format(int(m[2]),'.1f') + ', ' + format(int(m[3]),'.1f') + ')' + '\n' \
                    + 'Battery:  ' + format(float(m[4]) / 1000,'.3f') + '\n' \
                    + 'Temperature:  ' + format(float(m[5]),'.1f') + '\n' \
                    + 'Tilt:  ' + format(float(m[6]),'.1f')
       
        return result

    def get_measurement(self):        
        self.write(b''.join([b'GetMeasurement',bytes.fromhex('0d')]))       
        #return self.read()    
        return self.parse_measurement(self.read())

    def get_diagnostics(self):        
        self.write(b''.join([b'GetDiagnostic',bytes.fromhex('0d')]))
        return self.read()


class RubishBinSensor(Peripheral):
    def __init__(self,addr):
        Peripheral.__init__(self,addr)
        #Peripheral.__init__(self,None)
        #self.connect(addr)
    
        # Performs bluetooth discovery and populates self.services
        self.discoverServices()

        for service in self.services:
            if service.uuid == UUID(0x180A):    # Device Information service                               
                characteristics = service.getCharacteristics()
                for characteristic in characteristics:                    
                    if characteristic.uuid == UUID(AssignedNumbers.manufacturer_name_string):
                        self.manufacturer = characteristic.read().decode('utf-8')
                    elif characteristic.uuid == UUID(AssignedNumbers.model_number_string):
                        self.model_number = characteristic.read().decode('utf-8')
                    elif characteristic.uuid == UUID(AssignedNumbers.firmware_revision_string):
                        self.firmware_version = characteristic.read().decode('utf-8')
                    elif characteristic.uuid == UUID(AssignedNumbers.software_revision_string):
                        self.software_version = characteristic.read().decode('utf-8')

            elif service.uuid == UUID(0x1800):  # Generic Access service               
                characteristics = service.getCharacteristics()
                for characteristic in characteristics:
                    #print(characteristic)
                    if characteristic.uuid == UUID(AssignedNumbers.device_name):                    
                        if characteristic.supportsRead():
                            #print(characteristic.uuid)
                            #print(UUID(AssignedNumbers.device_name))
                            self.sensor_id = characteristic.read().decode('utf-8')

        self.serial = Serial(self)


class SerialDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)
        self.last_handle = None
        self.last_value = None

    def handleNotification(self, hnd, data):        
        #print(hnd)  # 25
        #print(data)
        
        self.last_handle = hnd
        self.last_value = data  # Might be only part of the data!       


def main():
    import time
    import sys
    import argparse
    import math

    parser = argparse.ArgumentParser()
    parser.add_argument('host', action='store',help='MAC of BLE device')
    parser.add_argument('-n', action='store', dest='count', default=0,
            type=int, help='Number of times to poll the sensor')
    parser.add_argument('-t',action='store',type=float, default=60.0, help='time between polling in seconds')

    arg = parser.parse_args(sys.argv[1:])

    host = arg.host
    host = host.strip().lower()

    found = False

    scanner = Scanner().withDelegate(ScanDelegate())
    for i in range(1,4,1):  # Try to discover the device 3 times
        print('Attempting to discover a Sensoneo device')
        devices = scanner.scan(10.0)    # blocking
        for device in devices:
            if (device.addr == host):
                if (device.connectable):
                    found = True
                    print('MAC %s is a Sensoneo device with Id %s' % (device.addr, device.getValueText(9)))                    
                else:
                    print('MAC %s is a Sensoneo device but it is not currently connectable' % (device.addr))
                    
                break
                    
        if (found):
            break
        else:
            print('Failed to discover a Sensoneo device on attempt ' + str(i) + '. Trying again in 10 seconds')
            time.sleep(10.0)    # Wait 10s before trying again

    if (found):    
        rubish_bin = None  

        counter = 1
        enabled = False
        connected = False
        try:
            while True:           
                if (not connected):
                    print('Connecting to ' + host)                        
                    rubish_bin = RubishBinSensor(host)  # This will throw an exception                    

                    if 'conn' in rubish_bin.getState() and 'u-blox' in rubish_bin.manufacturer:                            
                        print('Connected')
                        print(rubish_bin.sensor_id)                    

                        ''' print(rubish_bin.sensor_id)
                        print(rubish_bin.manufacturer)
                        print(rubish_bin.model_number)
                        print(rubish_bin.firmware_version)
                        print(rubish_bin.software_version) '''

                        rubish_bin.setDelegate(SerialDelegate())
                        enabled = rubish_bin.serial.enable()
                        print('Sending the pin')
                        connected = rubish_bin.serial.send_pin()
                        if (not connected): # Timeout situation
                            print('Failed to connect to ' + host + '. Try again in 30 seconds')
                            break                        
                    else:
                        print('Failed to connect to ' + host + '. Try again in 30 seconds')
                        break                    
          
                if (enabled) and (connected):                
                    print('Taking a measurement')
                    response = rubish_bin.serial.get_measurement()
                    if (response != ''):
                        print(response)
                    else:
                        print('No measurement data was received. Try again in 30 seconds')
                        break                    
                else:
                    break       
                
                if counter >= arg.count:
                    break
                counter += 1

                for i in range(0,int(arg.t),1):
                    if (math.ceil(math.modf(i / 5)[0]) == 0):
                        response = rubish_bin.serial.keep_alive() # keep alive
                        if (response != ''):
                            #print(response)
                            print('COMMUNICATIONS REFRESH')

                    time.sleep(1.0)
        except (BTLEDisconnectError, BTLEInternalError):
            if (not connected):
                print('Failed to connect to ' + host + '. Try again in 30 seconds')
            else:
                print('BLE communications have been lost to ' + host + '. Try again in 30 seconds')
        finally:            
            if (enabled):
                rubish_bin.serial.disable()
                rubish_bin.disconnect()
                enabled = False
                connected = False
        
            if (rubish_bin != None):
                del rubish_bin
    else:
        print('No Sensoneo devices were discovered')


if __name__ == '__main__':
    main()
