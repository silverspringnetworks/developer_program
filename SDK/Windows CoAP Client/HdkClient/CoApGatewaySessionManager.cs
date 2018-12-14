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
using EXILANT.Labs.CoAP.Settings;
using EXILANT.Labs.CoAP.Channels;
using EXILANT.Labs.CoAP.Message;
using EXILANT.Labs.CoAP.Helpers;

namespace HdkClient
{
    /// <summary>
    /// This class establishes and manages Gateway sessions.
    /// Once a session is open, the underlying socket is used as a vehicle for all subsequent CoAP calls.
    /// Methods are included here for initiating and terminating a session.
    /// Note: Sessions, unless terminated, still exist on the Gateway server.  A maximum of 10 are allowed, and each
    /// has a four hour expiry.  There is no way, once the session is left, to terminate it, so care should be taken to 
    /// manage sessions, closing them down at the end of each test.
    /// </summary>
    public class CoApGatewaySessionManager : CoApBase
    {
        private static CoApGatewaySessionManager __Instance = null;

        private bool __SessionEstablished = false;
        private bool __SessionTerminated = false;
        private bool __SessionRequestSucceeded = false;


        public static CoApGatewaySessionManager Instance
        {
            get
            {
                if (__Instance == null)
                {
                    __Instance = new CoApGatewaySessionManager();
                }

                return __Instance;
            }
        }

        /// <summary>
        /// Clientreturns the CoAPClientChallenge object containing the currently open socket.
        /// This can be used to make subsequent CoAP calls once a session has been established.
        /// </summary>
        public CoAPClientChannel Client
        {
            get
            {
                if (__coapClient == null)
                {
                    FileLogger.Write("CoAPClientChannel is null - creating new instance");
                    __coapClient = new CoAPClientChannel();
                    __coapClient.Initialize(GatewaySettings.Instance.GatewayURI, __ServerPort);
                    __coapClient.AckTimeout = __Timeout;
                }
                return __coapClient;
            }
        }
        /// <summary>
        /// ShutDown terminates any open Gateway session, and cleans up the local socket.
        /// </summary>
        public void ShutDown()
        {
            TerminateSession();
            FileLogger.Write("Shutting down session manager");
            __Done.Reset();
            __Done.Close();
            __Done = null;

            __coapClient.Shutdown();
            __coapClient = null;
        }
        /// <summary>
        /// The SessionEstablished property is a boolean indicating whether we have already
        /// created a Gateway session.
        /// </summary>
        public bool SessionEstablished
        {
            get { return true; }
        }
        /// <summary>
        /// TerminaeSession closes any open Gateway session.
        /// </summary>
        /// <returns></returns>
        public bool TerminateSession()
        {
            if (__SessionTerminated)
                return true;
            if (!__SessionEstablished)
                return true;
            FileLogger.Write("Terminating current session");
            __SessionTerminated = SessionCall(CoAPMessageCode.DELETE);
            __SessionEstablished = false;
            FileLogger.Write("Session terminated = " + __SessionTerminated.ToString());
            return __SessionTerminated;
        }

        /// <summary>
        /// IsSessionEstablished - returns true if status shows SessionEstablished.  
        /// State may be stale and you need to terminate and re-establish.
        /// </summary>
        /// <returns></returns>
        public bool IsSessionEstablished()
        {
            return __SessionEstablished;
        }

        /// <summary>
        /// EstablishSession passes the user's credentials to the Gateway.
        /// If the credentials are accepted, the related socket is left open and 
        /// subsequent CoAP calls are made using that socket.
        /// </summary>
        /// <returns></returns>
        public bool EstablishSession()
        {
            if (__SessionEstablished)
                return true;
            FileLogger.Write("Establishing new session");
            __SessionEstablished = SessionCall(CoAPMessageCode.POST);
            if (__SessionEstablished)
            {
                __SessionTerminated = false;
            }

            FileLogger.Write("Session established = " + __SessionEstablished.ToString());
            return __SessionEstablished;
        }
        /// <summary>
        /// Perform a session request (acquire or terminate).
        /// </summary>
        /// <param name="messageCode">CoAPMessageCode.POST for establishing a session or oAPMessageCode.DELETE for terminating a session</param>
        /// <returns>a boolean indicating success or failure of the request</returns>
        private bool SessionCall(byte messageCode)
            {
            __TimedOut = true;
            __SessionRequestSucceeded = false;
            string token = GatewaySettings.Instance.SfdpToken;
            //api.coap-test.developer.ssni.com
            string serverIP = AbstractNetworkUtils.GetIPAddressFromHostname(GatewaySettings.Instance.GatewayURI).ToString();

            __coapClient.Initialize(GatewaySettings.Instance.GatewayURI, __ServerPort);
            __coapClient.AckTimeout = __Timeout;
            __coapClient.CoAPResponseReceived += new CoAPResponseReceivedHandler(OnCoAPSessionResponseReceived);
            __coapClient.CoAPError += new CoAPErrorHandler(OnCoAPError);

            coapReq = new CoAPRequest(this.ConfirmableMessageType, 
                                                messageCode, 
                                                HdkUtils.MessageId());
            string uriToCall = "coap://" + GatewaySettings.Instance.GatewayURI + ":"
                + __ServerPort + "/sessions?security=none";//?token="
                //+ GatewaySettings.Instance.SfdpToken;
                //+ "&security=none";

            coapReq.SetURL(uriToCall);
            byte[] zero = { 0x00 };
            coapReq.AddOption(CoAPHeaderOption.CONTENT_FORMAT, zero);
            if (messageCode != CoAPMessageCode.DELETE)
            {
                coapReq.Payload = new CoAPPayload(GatewaySettings.Instance.SfdpToken);
            }
            SetToken();
            
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_QUERY);  // JJK - Change in v2.0.7

            FileLogger.Write("About to send session request");
            try
            {
                FileLogger.Write(coapReq.ToString());
            }
            catch(Exception flErr)
            {
                FileLogger.Write(flErr.Message);
            }
            __coapClient.Send(coapReq);
            __Done.WaitOne(GatewaySettings.Instance.RequestTimeout);
            __Done.Reset();
            if (__TimedOut)
            {
                this.ErrorResult = "Request timed out";
                FileLogger.Write(this.ErrorResult);
            }
            __coapClient.CoAPResponseReceived -= new CoAPResponseReceivedHandler(OnCoAPSessionResponseReceived);

            return __SessionRequestSucceeded;
            }

        /// <summary>
        /// Called when a response is received against a sent request
        /// </summary>
        /// <param name="coapResp">The CoAPResponse object</param>
        private void OnCoAPSessionResponseReceived(CoAPResponse coapResp)
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
                int classCode = ((coapResp.Code.Value & 0xE0) >> 5);
                if (classCode == 2)
                {
                    __SessionRequestSucceeded = true;
                    FileLogger.Write("Session request completed successfully");
                }
                else
                {
                    FileLogger.Write("Session call code " + coapResp.Code.Value.ToString());
                    //Will come here if an error occurred..
                }
                __Done.Set();
            }
        }

        /// <summary>
        /// Called when error occurs
        /// </summary>
        /// <param name="e">The exception that occurred</param>
        /// <param name="associatedMsg">The associated message (if any)</param>    
        private void OnCoAPError(Exception e, AbstractCoAPMessage associatedMsg)
        {
            __TimedOut = false;
            FileLogger.Write(e.Message);
            __Done.Set();
        }

    }
}
