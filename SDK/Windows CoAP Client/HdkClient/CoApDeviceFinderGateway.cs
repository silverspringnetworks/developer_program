/*
Silver Spring Networks, Inc. ("Silver Spring") licenses this software to you and the company (“You” or “Your”) 
a license for this software (“Software”).  
Using the Software constitutes Your acceptance of these terms (“License”).  

License Grant. Subject to Your acceptance and compliance with this License, Silver Spring grants to You, solely for Your own internal business purpose, 
               a non-exclusive, non-transferable license to access and use the Software and the associated user documentation (“Documentation”) 
               for the term and number agreed to be Silver Spring. 

Restrictions. No intellectual property rights are granted under this License and Silver Spring reserves all rights not expressly granted. 
You may not:  
(a) modify or create derivative works of the Software or Documentation; 
(b) assign, transfer, lease or sublicense the Software or Documentation to any third party 
    (other than Your consultants who are bound to written obligations of confidentiality at least as restrictive as those contained in this License); 
and (c) reverse engineer, disassemble, decrypt, extract or otherwise reduce the Software to a human perceivable 
    form or otherwise attempt to determine the source code or algorithms of the Software 
    (unless the foregoing restriction is expressly prohibited by applicable law).
You may not remove or destroy any proprietary, trademark or copyright markings or notices placed on or contained in the Software or Documentation.  
Silver Spring PROVIDES THE SOFTWARE “AS IS” AND MAKES NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY REGARDING OR RELATING TO THE SOFTWARE.  
Silver Spring HEREBY DISCLAIMS ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, 
WHETHER RIGHTS ARISES BY LAW, BY USE AND CUSTOM OR USAGE IN THE TRADE, OR BY COURSE OF DEALING. 
Silver Spring DOES NOT WARRANT THAT THE SOFTWARE OR ANY PORTION THEREOF WILL BE ERROR-FREE.

Termination. This License is effective until terminated. Your rights under this License will terminate automatically without notice if you fail to comply with any of its terms. 
Upon the termination of this License, You must cease all use of the Software and remove it from Your computer.
*/
using System;
using System.Collections;
using System.Collections.Generic;
using SLDPAPI;
using Newtonsoft.Json;
using EXILANT.Labs.CoAP.Settings;
using EXILANT.Labs.CoAP.Message;

namespace HdkClient
{
    /// <summary>
    /// Implementation for the Gateway registered CoAP devices.
    /// </summary>
    public class CoApDeviceFinderGateway : CoApDeviceFinder
    {
        /// <summary>
        /// Find all Gateway registered CoAP devices.
        /// </summary>
        /// <returns>A list of CoApDevice objects representing all registered devices</returns>
        public override CoApDevices LoadNodes()
        {
            CoAPSettings.Instance.AddressFamily = System.Net.Sockets.AddressFamily.InterNetworkV6;//.InterNetwork;
            CoApDevices devices = new CoApDevices();
            
            DevicesResponse jsonDevices;

            // Use SLDP API to fetch a list of devices.
            jsonDevices = GetDevices();
            if (jsonDevices != null)
            {
                foreach(SLDPAPI.DevicesResponse.Device dev in jsonDevices.devices)
                {
                    if (dev != null)
                    {
                        if (dev.deviceType == "MILLI")// && dev.domainInfo.nic_macID == "001350050047dc1f")// || dev.domainInfo.nic_macID == "001350050047dd88")
                        {
                            string mac = dev.domainInfo.nic_macID;
                            FileLogger.Write("Processing MAC " + mac);
                            CoApDevice d = new CoApDevice(mac);
                            devices.Add(d);
                            // If the ping fails then tag it as unreachable
                            //if (!j.Succeeded)
                            //{
                            //    d.Name = "UNREACHABLE - " + d.Name;
                            //    d.Reachable = false;
                            //}
                            //else
                            //{
                            d.Reachable = true;
                            //}
                        }
                    }
                }
                //if (jsonDevices.deviceSearch.queryResults != null)
                //{
                //    // Look through all discovered devices and add non-AP devices to the device list.
                //    for (int i = 0; i < jsonDevices.deviceSearch.queryResults.Count; i++)
                //    {

                //        FileLogger.Write("Processing MAC " + jsonDevices.deviceSearch.queryResults[i].nic_macId);
                //        if (jsonDevices.deviceSearch.queryResults[i].nic_imageType == "ACCESS_POINT")
                //        {
                //            FileLogger.Write("Ignoring access point " + jsonDevices.deviceSearch.queryResults[i].nic_macId);
                //        }

                //        // Ignore access points
                //        if (jsonDevices.deviceSearch.queryResults[i].nic_imageType != "ACCESS_POINT")
                //        {
                //            CoApDevice d = new CoApDevice(jsonDevices.deviceSearch.queryResults[i].nic_macId);
                //            devices.Add(d);
                //            // Ping the device
                //            //string ping = PingMacOLD(d.Name);
                //            //// Translate the ping JSON response into an equivalent C# object
                //            //PingResponseGateway j = new PingResponseGateway(ping);
                //            //// If the ping fails then tag it as unreachable
                //            //if (!j.Succeeded)
                //            //{
                //            //    d.Name = "UNREACHABLE - " + d.Name;
                //            //    d.Reachable = false;
                //            //}
                //            //else
                //            //{
                //            //    d.Reachable = true;
                //            //}
                //            bool ping = PingMac(d.Name);
                //            // If the ping fails then tag it as unreachable
                //            if (!ping)
                //            {
                //                d.Name = "UNREACHABLE - " + d.Name;
                //                d.Reachable = false;
                //            }
                //            else
                //            {
                //                d.Reachable = true;
                //            }
                //        }
                //    }
                //}
            }

            return devices;
        }
        /// <summary>
        /// Ping device using the Gateway.
        /// </summary>
        /// <param name="mac">the mac of the device to ping</param>
        /// <returns>a boolean indicating whether the ping was successful</returns>
        private bool PingMac(string mac)
        {
            // There are some considerations with respect to 
            return true;
        }
        /// <summary>
        /// Fetch a list of devices, in JSON format, from the Gateway.
        /// </summary>
        /// <returns>A JSON document containing the available devices.</returns>
        public DevicesResponse GetDevices()
        {


            var json = "";

            SLDPAPI.SilverLink link = new SilverLink();

            // Set up the SLDP API request.
            link.ClientID = GatewaySettings.Instance.ClientId;
            link.Secret = GatewaySettings.Instance.ClientSecret;
            link.Solution = GatewaySettings.Instance.Solution;
            link.BaseURL = GatewaySettings.Instance.GatewayBaseURL;
            link.TokenPath = GatewaySettings.Instance.GatewayTokens;

            FileLogger.Write("About to send Gateway request");
            FileLogger.Write("Customer Id = " + GatewaySettings.Instance.Solution);
            FileLogger.Write("Base URL = " + GatewaySettings.Instance.GatewayBaseURL);
            FileLogger.Write("Path = " + GatewaySettings.Instance.GatewayDeviceSearch);

            try
            {
                string path = GatewaySettings.Instance.GatewayDeviceSearch.Replace("<solution>", GatewaySettings.Instance.Solution);
                FileLogger.Write("About to send Gateway request");
                FileLogger.Write("Customer Id = " + GatewaySettings.Instance.Solution);
                FileLogger.Write("Base URL = " + GatewaySettings.Instance.GatewayBaseURL);
                FileLogger.Write("Path = " + path);
                json = link.Get(path);
            }
            catch(Exception e)
            {
                FileLogger.Write("Get failed");
                FileLogger.Write(e.Message);
                FileLogger.Write(e.StackTrace);

                // Rethrow the error if the POST failed.
                throw e;
            }
            FileLogger.Write("JSON response = " + json);
            
            GatewaySettings.Instance.SfdpToken = link.SilverLinkToken;
            FileLogger.Write("Token acquired from SFDP");
            FileLogger.Write(GatewaySettings.Instance.SfdpToken);

            DevicesResponse devices = null;
            try
            {
                devices = JsonConvert.DeserializeObject<DevicesResponse>(json);
            }
            catch(Exception jsonDeserializeError)
            {
                FileLogger.Write("Error deserializing device search response");
                FileLogger.Write(jsonDeserializeError.Message);
                FileLogger.Write(jsonDeserializeError.Message);
            }

            return devices;
        }

    }
}
