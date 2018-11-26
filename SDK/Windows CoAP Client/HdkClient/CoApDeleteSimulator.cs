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
using EXILANT.Labs.CoAP.Channels;
using EXILANT.Labs.CoAP.Message;
using EXILANT.Labs.CoAP.Helpers;

namespace HdkClient
{
    //class CoApDeleteSimulator
    /// <summary>
    /// Simulator implementation of the CoAP DELETE feature.
    /// </summary>
    public class CoApDeleteSimulator : CoApDelete
    {
        /// <summary>
        /// Send the request to the CoAP server.
        /// </summary>
        public override void Send()
        {
            string device = __IpAddress;
            string serverIP = "localhost";
            __coapClient.Initialize(serverIP, __ServerPort);

            __coapClient.Initialize(serverIP, __ServerPort);
            __coapClient.CoAPResponseReceived += new CoAPResponseReceivedHandler(OnCoAPResponseReceived);

            __coapClient.CoAPError += new CoAPErrorHandler(OnCoAPError);

            coapReq = new CoAPRequest(this.ConfirmableMessageType,
                                                CoAPMessageCode.DELETE,
                                                HdkUtils.MessageId());

            string uriToCall = "coap://" + serverIP + ":" + __ServerPort + "/" + __IpAddress + __URI;

            coapReq.SetURL(uriToCall);
            SetToken();
            // Send out the request.
            byte[] octet = new byte[1];
            // As of this moment, the expectation is that the content is an octet stream.
            octet[0] = (byte)CoAPContentFormatOption.APPLICATION_OCTET_STREAM;
            coapReq.Options.AddOption(CoAPHeaderOption.PROXY_SCHEME, "coap");
            coapReq.Options.AddOption(CoAPHeaderOption.CONTENT_FORMAT, octet);
            coapReq.Payload = __Payload;
            FileLogger.Write("About to send CoAP request");
            FileLogger.Write(coapReq.ToString());
            __coapClient.Send(coapReq);

            __Done.WaitOne(GatewaySettings.Instance.RequestTimeout);
            __Done.Reset();
            CoApGatewaySessionManager.Instance.Client.CoAPResponseReceived -= new CoAPResponseReceivedHandler(OnCoAPResponseReceived);
            CoApGatewaySessionManager.Instance.Client.CoAPError -= new CoAPErrorHandler(OnCoAPError);
        }
        /// <summary>
        /// Called when error occurs
        /// </summary>
        /// <param name="e">The exception that occurred</param>
        /// <param name="associatedMsg">The associated message (if any)</param>    
        private void OnCoAPError(Exception e, AbstractCoAPMessage associatedMsg)
        {
            FileLogger.Write(e.Message);
            __Done.Set();
        }

        /// <summary>
        /// Called when a response is received against a sent request
        /// </summary>
        /// <param name="coapResp">The CoAPResponse object</param>
        private void OnCoAPResponseReceived(CoAPResponse coapResp)
        {
            string tokenRx = (coapResp.Token != null && coapResp.Token.Value != null) ? AbstractByteUtils.ByteToStringUTF8(coapResp.Token.Value) : "";
            FileLogger.Write("Received response from server - token = " + tokenRx);
            FileLogger.Write(coapResp.ToString());
            __Response = coapResp;
            if (tokenRx == __Token)
            {
                if (BreakIfError(coapResp.Code.Value))
                {
                    return;
                }

                if (coapResp.Code.Value == CoAPMessageCode.DELETED)
                {

                    __Done.Set();
                }
                else
                {
                    //Will come here if an error occurred..
                }
            }

        }
    }
}
