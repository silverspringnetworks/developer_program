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
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HdkClient
{
    /// <summary>
    /// Static settings related to Gateway calls.
    /// </summary>
    public class GatewaySettings
    {
        private static GatewaySettings __Instance = null;
        private static string __ClientId = "";
        private static string __ClientSecret = "";
        private static string __Solution = "";

        private static string __GatewayBaseURL = "https://poc.api.ssniot.cloud/api";//"https://api.silverlink.silverspringnet.com";// "https://api.sdp.ssnsgs.net";
        private static string __GatewayTokens = "/api/tokens";
        private static string __GatewayPingPath = "/sdp/v1/ping/";
        private static string __GatewaySolutionSearch = "/api/solutions";
        private static string __GatewayDeviceSearch = "/api/solutions/<solution>/devices";

        private string __SfdpToken = "";

        private static string __GatewayURI = "COAPAPI.SSNI.COM";
        private static string __GatewayDeviceURIPrefix = "SSN";
        private static string __GatewayDeviceURISuffix = ".SG.PROD.STAR.SSNSGS.NET";

        private static int __CoApPort = 5683;
        private static int __RequestTimeout = 40000;
        /// <summary>
        /// Instance is the property that ensures a singleton instance of this class.
        /// Note that this is a "lazy" implementation.  An object of this type is not created
        /// until required by the application.
        /// </summary>
        public static GatewaySettings Instance
        {
            get
            {
                if (__Instance == null)
                {
                    __Instance = new GatewaySettings();
                }

                return __Instance;
            }
        }
        /// <summary>
        /// Clear all credentials, forcing a new set of credentials to be requested.
        /// </summary>
        public void ClearCredentials()
        {
            __ClientId = "";
            __ClientSecret = "";
            __Solution = "";
            __SfdpToken = "";
    }
        /// <summary>
        /// SLDP client id
        /// </summary>
    public string ClientId 

        {
            get { return __ClientId; }
            set { __ClientId = value; }
        }
        /// <summary>
        /// SLDP client password
        /// </summary>
        public string ClientSecret
        {
            get { return __ClientSecret; }
            set { __ClientSecret = value; }
        }
        /// <summary>
        /// SFDP solution
        /// </summary>
        public string Solution
        {
            get { return __Solution; }
            set { __Solution = value; }
        }
        /// <summary>
        /// SFDP Token
        /// </summary>
        public string SfdpToken
        {
            get { return __SfdpToken; }
            set { __SfdpToken = value; }
        }

        /// <summary>
        /// CoAP Gateway URI for all calls
        /// </summary>
        public string GatewayURI
        {
            get { return __GatewayURI; }
            set { __GatewayURI = value; }
        }
        /// <summary>
        /// Base URL for SLDP calls
        /// </summary>
        public string GatewayBaseURL
        {
            get { return __GatewayBaseURL; }
            set { __GatewayBaseURL = value; }
        }
        /// <summary>
        /// SLDP path specification for ping requests
        /// </summary>
        public string GatewayPingPath
        {
            get { return __GatewayPingPath; }
            set { __GatewayPingPath = value; }
        }
        /// <summary>
        /// SLDP path specification for obtaining a list of registered solutions
        /// </summary>
        public string GatewaySolutionSearch
        {
            get { return __GatewaySolutionSearch; }
            set { __GatewaySolutionSearch = value; }
        }
        /// <summary>
        /// SLDP path specification for obtaining a list of registered devices
        /// </summary>
        public string GatewayDeviceSearch
        {
            get { return __GatewayDeviceSearch; }
            set { __GatewayDeviceSearch = value; }
        }
        /// <summary>
        /// For the CoAP Gateway, we translate a MAC into a URI.
        /// GatewayDeviceURIPrefix represents the string pre-pended to the MAC. 
        /// </summary>
        public string GatewayDeviceURIPrefix
        {
            get { return __GatewayDeviceURIPrefix; }
            set { __GatewayDeviceURIPrefix = value; }
        }
        /// <summary>
        /// For the CoAP Gateway, we translate a MAC into a URI.
        /// GatewayDeviceURIPrefix represents the string appended to the MAC.         
        /// </summary>
        public string GatewayDeviceURISuffix
        {
            get { return __GatewayDeviceURISuffix; }
            set { __GatewayDeviceURISuffix = value; }
        }
        public string GatewayTokens
        {
            get { return __GatewayTokens; }
            set { __GatewayTokens = value; }
        }


        /// <summary>
        /// The port to be used for CoAP calls.
        /// </summary>
        public int CoApPort
        {
            get { return __CoApPort; }
            set { __CoApPort = value; }
        }
        /// <summary>
        /// The request timeout for CoAP Gateway calls.
        /// Note that this value is currently defaulted to the same value as the CoAP server timeout.
        /// </summary>
        public int RequestTimeout
        {
            get { return __RequestTimeout; }
            set { __RequestTimeout = value; }
        }
        public void Persist()
        {
            Properties.Settings.Default.DevicePrefix = __GatewayDeviceURIPrefix;
            Properties.Settings.Default.DeviceSuffix = __GatewayDeviceURISuffix;
            Properties.Settings.Default.GatewayUri = __GatewayURI;
            Properties.Settings.Default.GatewayUrl = __GatewayBaseURL;
            Properties.Settings.Default.Save();
        }
        public void Restore()
        {
            __GatewayDeviceURIPrefix = Properties.Settings.Default.DevicePrefix;
            __GatewayDeviceURISuffix = Properties.Settings.Default.DeviceSuffix;
            __GatewayURI = Properties.Settings.Default.GatewayUri;
            __GatewayBaseURL = Properties.Settings.Default.GatewayUrl;
        }
    }
}