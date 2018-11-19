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
using System.Reflection;

namespace HdkClient
{
    /// <summary>
    /// This class provides a layer of indirection surrounding the instantiation of 
    /// key HDK client classes.  It also captures current network interface used.
    /// </summary>
    public class NetworkInterface
    {
        /// <summary>
        /// Enumerator listing all supported network interfaces.
        /// </summary>
        public enum NetworkOption
        {
            Gateway,
            Simulator
        }

        private static NetworkInterface __Instance = null;
        private static NetworkOption __InterfaceMethod;

        /// <summary>
        /// Instance is the property that ensures a singleton instance of this class.
        /// Note that this is a "lazy" implementation.  An object of this type is not created
        /// until required by the application.
        /// </summary>
        public static NetworkInterface Instance
        {
            get
            {
                if (__Instance == null)
                {
                    __Instance = new NetworkInterface();
                }

                return __Instance;
            }
        }

        /// <summary>
        /// Property that identifies currently used by the application.
        /// </summary>
        public NetworkOption SelectedInterface
        {
            get { return __InterfaceMethod; }
            set { __InterfaceMethod = value; }
        }

        /// <summary>
        /// Boolean property indicating whether or not the LAN interface is being used.
        /// </summary>
        public bool UsingSimulator
        {
            get { return __InterfaceMethod == NetworkOption.Simulator; }
        }
        /// <summary>
        /// Boolean property indicating whether or not the FSU interface is being used.
        /// </summary>
        //public bool UsingFSU
        //{
        //    get { return __InterfaceMethod == NetworkOption.Fsu; }
        //}
        /// <summary>
        /// Boolean property indicating whether or not the Gateway interface is being used.
        /// Note that this is the only supported interface in this version of the code.
        /// </summary>
        public bool UsingGateway
        {
            get { return __InterfaceMethod == NetworkOption.Gateway; }
        }

        /// <summary>
        /// 
        /// </summary>
        public object DiscoveryRequest
        {
            get
            {
                object o = (CoApDiscovery)ObjectLoader("CoApDiscovery");
                return o;
            }

        }
        public object NodeFinder
        {
            get
            {
                object o = (CoApDeviceFinder)ObjectLoader("CoApDeviceFinder");
                return o;
            }

        }
        public object ResourceGetter
        {
            get
            {
                object o = (CoApGet)ObjectLoader("CoApGet");
                return o;
            }

        }
        public object ResourcePinger
        {
            get
            {
                object o = (CoApPing)ObjectLoader("CoApPing");
                return o;
            }

        }

        public object ResourcePutter
        {
            get
            {
                object o = (CoApPut)ObjectLoader("CoApPut");
                return o;
            }
        }

        public object ResourceDeleter
        {
            get
            {
                object o = (CoApDelete)ObjectLoader("CoApDelete");
                return o;
            }

        }
        /// <summary>
        /// Uses reflection to instantiate a core HDK client class.
        /// </summary>
        /// <param name="classPrefix">a string describing the base class of the object to return</param>
        /// <returns></returns>
        private object ObjectLoader(string classPrefix)
        {
            // All classes come from the currently executing assembly.
            Assembly assem = System.Reflection.Assembly.GetExecutingAssembly();
            FileLogger.Write(String.Format("Assembly name: {0}", assem.FullName));

            string thisNamespace = assem.FullName.Substring(0, assem.FullName.IndexOf(','));
            Type t = assem.GetType(thisNamespace + "." + classPrefix + __InterfaceMethod.ToString());

            object o = Activator.CreateInstance(t);
            FileLogger.Write(String.Format("Object returned: {0}", o.ToString()));
            return o;
        }
    }
}
