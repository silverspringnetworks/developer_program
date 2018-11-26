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
using EXILANT.Labs.CoAP.Helpers;
namespace HdkClient
{
    public class CoApDiscoveryGateway : CoApDiscovery
    {
        /// <summary>
        /// Load resources associated with a specific device.
        /// This is the CoAP Gateway implementation of CoApDiscovery.
        /// </summary>
        /// <returns>a collection of CoApResource objects</returns>
        /// 
        public override CoApResources LoadResources()
        {
            // If we can't establish a socket with a good login to the gateway api, then just return;
            bool gotSession = false;
            try
            {
                gotSession = CoApGatewaySessionManager.Instance.EstablishSession();
            }
            catch(Exception noSession)
            {
                FileLogger.Write("Error obtaining session");
                FileLogger.Write(noSession.Message);
                FileLogger.Write(noSession.StackTrace);
                this.ErrorResult = "Error obtaining session" + noSession.Message;
            }

            if (!gotSession)
            {
                FileLogger.Write("Session NOT Established");
                return null;
            }

            // This represents the device being examined.
            string serverIP = __IpAddress;

            // Set up the notification handlers.
            CoApGatewaySessionManager.Instance.Client.CoAPResponseReceived += new CoAPResponseReceivedHandler(OnCoAPDiscoveryResponseReceived);
            CoApGatewaySessionManager.Instance.Client.CoAPError += new CoAPErrorHandler(OnCoAPDiscoveryError);
            __TimedOut = true;

            // Discovery requires confirmation and a message type of GET
            coapReq = new CoAPRequest(this.ConfirmableMessageType,
                                                CoAPMessageCode.GET, 
                                                HdkUtils.MessageId());

            string uriToCall = "coap://" + UriFromMac(__IpAddress) + ":" + __ServerPort + "/.well-known/core";//"/.well-known/core";
            coapReq.SetURL(uriToCall);
            SetToken();
            // Send out the coap request

            // JJK - Change in v2.0.7
            //coapReq.Options.AddOption(CoAPHeaderOption.PROXY_SCHEME, "coap");
            // make Proxy packet Change in v2.0.7
            coapReq.Options.AddOption(CoAPHeaderOption.BLOCK2, new byte[] { CoAPBlockOption.BLOCK_SIZE_128 });
            //coapReq.Options.AddOption(CoAPHeaderOption.PROXY_URI, "coap://SSN001350050047dc9a.SG.YEL01.SSN.SSNSGS.NET:4849/.well-known/core");
            coapReq.Options.AddOption(CoAPHeaderOption.PROXY_URI, "coap://" + UriFromMac(__IpAddress) + ":" + "4849" + "/.well-known/core");
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_HOST);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_QUERY);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_PATH);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_PORT);
            coapReq.Options.RemoveOption(CoAPHeaderOption.CONTENT_FORMAT);
            // end of Changes in v2.0.7

            FileLogger.Write("About to send Gateway resource request");
            FileLogger.Write(coapReq.ToString());
            CoApGatewaySessionManager.Instance.Client.Send(coapReq);

            CoApDiscoveryResponse response = null;
            // Wait for a response from the discovery request.
            // Time out after the pre-defined maximum wait time.
            __Done.WaitOne(GatewaySettings.Instance.RequestTimeout);
            response = this.DiscoveryResponse;

            // Reset the wait object
            __Done.Reset();
            if(__TimedOut)
            {
                this.ErrorResult = "Request timed out";
            }
            // Remove event handlers.
            CoApGatewaySessionManager.Instance.Client.CoAPResponseReceived -= new CoAPResponseReceivedHandler(OnCoAPDiscoveryResponseReceived);
            CoApGatewaySessionManager.Instance.Client.CoAPError -= new CoAPErrorHandler(OnCoAPDiscoveryError);
            return response.Resources;
        }
        /// <summary>
        /// Generates a Gateway-format URI based on the MAC being queried.
        /// </summary>
        /// <param name="mac">the mac of the target device</param>
        /// <returns>a URI to add to the device call</returns>
        private string UriFromMac(string mac)
        {
            return GatewaySettings.Instance.GatewayDeviceURIPrefix + mac + GatewaySettings.Instance.GatewayDeviceURISuffix;
        }

        /// <summary>
        /// Called when error occurs
        /// </summary>
        /// <param name="e">The exception that occurred</param>
        /// <param name="associatedMsg">The associated message (if any)</param>    
        private void OnCoAPDiscoveryError(Exception e, AbstractCoAPMessage associatedMsg)
        {
            __TimedOut = false;
            FileLogger.Write(e.Message);
            //Write your error logic here
            __Done.Set();
        }

        /// <summary>
        /// Called when a response is received against a sent request
        /// </summary>
        /// <param name="coapResp">The CoAPResponse object</param>
        private void OnCoAPDiscoveryResponseReceived(CoAPResponse coapResp)
        {
          __TimedOut = false;
            string tokenRx = "";// (coapResp.Token != null && coapResp.Token.Value != null) ? AbstractByteUtils.ByteToStringUTF8(coapResp.Token.Value) : "";
            if (coapResp != null)
                if (coapResp.Token != null)
                {
                    tokenRx = AbstractByteUtils.ByteToStringUTF8(coapResp.Token.Value);
                }
            try
            {
                FileLogger.Write("Received response from server - token = " + tokenRx);
                FileLogger.Write(coapResp.ToString());
            }
            catch { }

            if (tokenRx == __Token)
            {
                if (BreakIfError(coapResp.Code.Value))
                {
                    this.ErrorResult = coapResp.Code.ToString();
                    return;
                }

                if (coapResp.Code.Value == CoAPMessageCode.CONTENT)
                {
                    // Looks like we have the expected response.
                    // Check the content format and figure out how to store the response.
                    ArrayList options = coapResp.Options.GetOptions((ushort)CoAPHeaderOption.CONTENT_FORMAT);
                    if (options.Count > 0)
                    {
                        CoAPContentFormatOption ccformat = new CoAPContentFormatOption();
                        bool proceed = false;
                        foreach (CoAPHeaderOption o in options)
                        {
                            ccformat = new CoAPContentFormatOption(AbstractByteUtils.ToUInt16(o.Value));
                            if (ccformat.IsValid())
                            {
                                proceed = true;
                                break;
                            }
                        }
                        // The header options match what we expected.
                        // Set the discovery result.
                        if (proceed)
                        {
                            if (ccformat.Value == CoAPContentFormatOption.APPLICATION_LINK_FORMAT)
                            {
                                string discovery = AbstractByteUtils.ByteToStringUTF8(coapResp.Payload.Value);
                                __DiscoveryResult = discovery;
                            }

                            __Response = coapResp;
                            __Done.Set();
                        }
                    }
                }
                else
                {
                    //Will come here if an error occurred..
                }
            }
        }
    }
}
