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
using EXILANT.Labs.CoAP.Channels;
using EXILANT.Labs.CoAP.Message;
using System.Threading;
using EXILANT.Labs.CoAP.Helpers;

namespace HdkClient
{
    /// <summary>
    /// Implementation for the Simulator registered CoAP devices.
    /// </summary>
    public class CoApDeviceFinderSimulator : CoApDeviceFinder
    {
        private string __FindResult = "";
        public CoApDeviceFinderSimulator()
        {
            __IpAddress = "localhost";
        }
        /// <summary>
        /// Find all CoAP devices defined in the simulator.
        /// </summary>
        /// <returns>A list of CoApDevice objects representing all registered devices</returns>
        public override CoApDevices LoadNodes()
        {
            CoApDevices devices = new CoApDevices();
            RequestDeviceList();
            if (__FindResult != null)
            {
                string[] devs = __FindResult.Split(';');
                foreach (string dev in devs)
                {
                    CoApDevice d = new CoApDevice(dev);
                    devices.Add(d);
                    d.Reachable = true;
                }
            }

            return devices;
        }
        /// <summary>
        /// This method composes a UDP call to the simulator to acquire the device list.
        /// </summary>
        private void RequestDeviceList()
        {
            string serverIP = __IpAddress;

            __coapClient.Initialize(serverIP, __ServerPort);
            __coapClient.AckTimeout = __Timeout;
            __coapClient.CoAPResponseReceived += new CoAPResponseReceivedHandler(OnCoAPResponseReceived);
            __coapClient.CoAPRequestReceived += new CoAPRequestReceivedHandler(OnCoAPRequestReceived);
            __coapClient.CoAPError += new CoAPErrorHandler(OnCoAPError);
            //Send a NON request to get the temperature...in return we will get a NON request from the server
            coapReq = new CoAPRequest(this.ConfirmableMessageType, //CoAPMessageType.NON
                                                CoAPMessageCode.GET,
                                                HdkUtils.MessageId());//hardcoded message ID as we are using only once
            string uriToCall = "coap://" + serverIP + ":" + __ServerPort + "/devices";//"/sensors/temp";"/time";//
            coapReq.SetURL(uriToCall);
            __Token = DateTime.Now.ToString("HHmmss");//Token value must be less than 8 bytes
            coapReq.Token = new CoAPToken(__Token);//A random token
            __coapClient.Send(coapReq);
            __Done.WaitOne(GatewaySettings.Instance.RequestTimeout);
            __Done.Reset();
            __Done.Close();
            __Done = null;
            __coapClient.Shutdown();
            __coapClient = null;
        }

        /// <summary>
        /// Called when error occurs
        /// </summary>
        /// <param name="e">The exception that occurred</param>
        /// <param name="associatedMsg">The associated message (if any)</param>    
        private void OnCoAPError(Exception e, AbstractCoAPMessage associatedMsg)
        {
            Console.WriteLine(e.Message);
            //Write your error logic here
            __Done.Set();
        }

        /// <summary>
        /// Called when a request is received...
        /// </summary>
        /// <param name="coapReq">The CoAPRequest object</param>
        private void OnCoAPRequestReceived(CoAPRequest coapReq)
        {
            Console.WriteLine(coapReq.QueryString);
        }

        /// <summary>
        /// Called when a response is received against a sent request
        /// </summary>
        /// <param name="coapResp">The CoAPResponse object</param>
        private void OnCoAPResponseReceived(CoAPResponse coapResp)
        {
            string tokenRx = (coapResp.Token != null && coapResp.Token.Value != null) ? AbstractByteUtils.ByteToStringUTF8(coapResp.Token.Value) : "";
            if (tokenRx == __Token)
            {
                if (coapResp.Code.Value == CoAPMessageCode.CONTENT)
                {

                    ArrayList options = coapResp.Options.GetOptions((ushort)CoAPHeaderOption.CONTENT_FORMAT);
                    if (options.Count > 0)
                    {
                        bool proceed = false;
                        foreach (CoAPHeaderOption o in options)
                        {
                            CoAPContentFormatOption ccformat = new CoAPContentFormatOption(AbstractByteUtils.ToUInt16(o.Value));
                            if (ccformat.Value == CoAPContentFormatOption.APPLICATION_LINK_FORMAT)
                            {
                                proceed = true;
                            }
                        }
                        if (proceed)
                        {
                            string nodes = "";
                            if (coapResp.Payload != null)
                            {
                                nodes = AbstractByteUtils.ByteToStringUTF8(coapResp.Payload.Value);
                            }
                            Console.WriteLine("Find Nodes response " + __coapClient.EndPoint.ToString() + " = " + nodes);
                            __FindResult = nodes;
                        }
                    }
                }
                else
                {
                    //Will come here if an error occurred..
                }
            }
            __Done.Set();

        }
    }
}
