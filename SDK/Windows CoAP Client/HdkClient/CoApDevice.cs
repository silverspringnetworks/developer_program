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
using System.Collections.Generic;
using System.Windows.Forms;

namespace HdkClient
{
    /// <summary>
    /// Object representing the CoAP device to be managed.
    /// </summary>
    public class CoApDevice
    {
        private CoApResources __Resources = null;
        private string __DeviceName = "";
        private bool __Reachable = false;
        /// <summary>
        /// Default constructor
        /// </summary>
        public CoApDevice()
        {
        }
        /// <summary>
        /// Constructor setting the device name.
        /// </summary>
        /// <param name="name">the name of the device</param>
        public CoApDevice(string name)
        {
            __DeviceName = name;
        }
        /// <summary>
        /// The device name, typically a MAC or an IP address.
        /// </summary>
        public string Name
        {
            get { return __DeviceName; }
            set { __DeviceName = value; }
        }
        /// <summary>
        /// Sets or gets a boolean indicating whether the device can be 
        /// reached (typicall via a ping).
        /// </summary>
        public bool Reachable
        {
            get { return __Reachable; }
            set { __Reachable = value; }
        }
        /// <summary>
        /// This uses the __Reachable variable, but returns the opposite
        /// boolean value.
        /// </summary>
        public bool Unreachable
        {
            get { return !__Reachable; }
            set { __Reachable = !value; }
        }

        /// <summary>
        /// The resources related to a device.
        /// </summary>
        public CoApResources Resources
        {
            get { return __Resources; }
            set { __Resources = value; }
        }
    }
}
