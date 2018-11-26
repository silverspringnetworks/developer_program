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
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EXILANT.Labs.CoAP.Settings;
using EXILANT.Labs.CoAP.Channels;
using EXILANT.Labs.CoAP.Message;
using EXILANT.Labs.CoAP.Helpers;
using System.Windows.Forms;
using System.Threading;

namespace HdkClient
{
    /*   
   A GET request with an Observe Option set to 0 (register) requests the
   server not only to return a current representation of the target
   resource, but also to add the client to the list of observers of that
   resource.  Upon success, the server returns a current representation
   of the resource and MUST keep this representation updated (as
   described in Section 1.3) as long as the client is on the list of
   observers.

   The entry in the list of observers is keyed by the client endpoint
   and the token specified by the client in the request.  If an entry
   with a matching endpoint/token pair is already present in the list
   (which, for example, happens when the client wishes to reinforce its
   interest in a resource), the server MUST NOT add a new entry but MUST
   replace or update the existing one.

   A server that is unable or unwilling to add a new entry to the list
   of observers of a resource MAY silently ignore the registration
   request and process the GET request as usual.  The resulting response
   MUST NOT include an Observe Option, the absence of which signals to
   the client that it will not be notified of changes to the resource
   and, e.g., needs to poll the resource for its state instead.

   If the Observe Option in a GET request is set to 1 (deregister), then
   the server MUST remove any existing entry with a matching endpoint/
   token pair from the list of observers and process the GET request as
   usual.  The resulting response MUST NOT include an Observe Option.
   */
    public class CoApObserverGateway : CoApGet
    {
        private bool __Observing = false;
        public event ObserveResponseHandler ObserveResponseReceived;
        public delegate void ObserveResponseHandler(CoAPResponse coapResp);

        public bool Observing
        {
            get { return __Observing; }
        }
        public void StartObserving()
        {

            string serverIP = __IpAddress;

            // Set the response handlers.
            CoApGatewaySessionManager.Instance.Client.CoAPResponseReceived += new CoAPResponseReceivedHandler(OnCoAPObserveResponseReceived);
            CoApGatewaySessionManager.Instance.Client.CoAPError += new CoAPErrorHandler(OnCoAPError);

            coapReq = new CoAPRequest(this.ConfirmableMessageType, 
                                                CoAPMessageCode.GET,
                                                HdkUtils.MessageId());

            string uriToCall = "coap://" + HdkUtils.UriFromMac(__IpAddress) + ":" + __ServerPort + __URI;
            coapReq.SetURL(uriToCall);
            SetToken();

            string blat = coapReq.ToString();
            byte[] observerOption = new byte[1];
            observerOption[0] = 0; // Start observing
            coapReq.Options.AddOption(CoAPHeaderOption.OBSERVE, new byte[0]);

            //coapReq.Options.AddOption(CoAPHeaderOption.OBSERVE, observerOption);
            // JJK - Changes in v2.0.7
            //coapReq.Options.AddOption(CoAPHeaderOption.PROXY_SCHEME, "coap");
            // make Proxy packet
            coapReq.Options.AddOption(CoAPHeaderOption.BLOCK2, new byte[] { CoAPBlockOption.BLOCK_SIZE_128 });
            coapReq.Options.AddOption(CoAPHeaderOption.PROXY_URI, "coap://" + HdkUtils.UriFromMac(__IpAddress) + ":" + "4849" + __URI);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_HOST);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_QUERY);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_PATH);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_PORT);
            coapReq.Options.RemoveOption(CoAPHeaderOption.CONTENT_FORMAT);
            // JJK - end of Changes in v2.0.7

            FileLogger.Write("About to send CoAP request");
            FileLogger.Write(coapReq.ToString());
            CoApGatewaySessionManager.Instance.Client.Send(coapReq);

            __Observing = true;

            while (__Observing)
            {
                //FileLogger.Write(CoApGatewaySessionManager.Instance.Client.ToString());
                Application.DoEvents();
                Thread.Sleep(100);
                //__Done.WaitOne(GatewaySettings.Instance.RequestTimeout);
            }
            __Done.Reset();
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
        /// Send a "stop" observing message to the server for this specific observation.
        /// If the client no longer wants to listen to the notifications, it can send back an RST as a response on the next notification. 
        /// The server will then, in turn, de-register the client from notifications list.
        /// Every client that receives the CON message, is expected to send an ACK, to continuously receive the notifications, or to send a RST to de-register
        /// </summary>
        public void TerminateObserving()
        {
            __Done.Set();
            string serverIP = __IpAddress;

            // Set the response handlers.
            CoApGatewaySessionManager.Instance.Client.CoAPResponseReceived += new CoAPResponseReceivedHandler(OnCoAPObserveResponseReceived);
            CoApGatewaySessionManager.Instance.Client.CoAPError += new CoAPErrorHandler(OnCoAPError);

            coapReq = new CoAPRequest(this.ConfirmableMessageType,
                                                CoAPMessageCode.GET,
                                                HdkUtils.MessageId());

            string uriToCall = "coap://" + HdkUtils.UriFromMac(__IpAddress) + ":" + __ServerPort + __URI;
            coapReq.SetURL(uriToCall);
            coapReq.Token = new CoAPToken(__Token);//A random token

            byte[] observerOption = new byte[1];
            observerOption[0] = 1;// Stop observing
            coapReq.Options.AddOption(CoAPHeaderOption.OBSERVE, observerOption);
            // JJK - Changes in v2.0.7
            //coapReq.Options.AddOption(CoAPHeaderOption.PROXY_SCHEME, "coap");
            // make Proxy packet
            coapReq.Options.AddOption(CoAPHeaderOption.BLOCK2, new byte[] { CoAPBlockOption.BLOCK_SIZE_128 });
            coapReq.Options.AddOption(CoAPHeaderOption.PROXY_URI, "coap://" + HdkUtils.UriFromMac(__IpAddress) + ":" + "4849" + __URI);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_HOST);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_QUERY);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_PATH);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_PORT);
            coapReq.Options.RemoveOption(CoAPHeaderOption.CONTENT_FORMAT);
            // JJK - end of Changes in v2.0.7

            FileLogger.Write("About to send CoAP request");
            FileLogger.Write(coapReq.ToString());
            CoApGatewaySessionManager.Instance.Client.Send(coapReq);

            __Done.WaitOne(GatewaySettings.Instance.RequestTimeout);
            __Done.Reset();
            CoApGatewaySessionManager.Instance.Client.CoAPResponseReceived -= new CoAPResponseReceivedHandler(OnCoAPObserveResponseReceived);
            CoApGatewaySessionManager.Instance.Client.CoAPError -= new CoAPErrorHandler(OnCoAPError);

            __Observing = false; 
        }

        /// <summary>
        /// Called when a response is received against a sent request
        /// </summary>
        /// <param name="coapResp">The CoAPResponse object</param>
        private void OnCoAPObserveResponseReceived(CoAPResponse coapResp)
        {
            string tokenRx = (coapResp.Token != null && coapResp.Token.Value != null) ? AbstractByteUtils.ByteToStringUTF8(coapResp.Token.Value) : "";
            FileLogger.Write("Received response from server - token = " + tokenRx);
            FileLogger.Write(coapResp.ToString());
            if (tokenRx == __Token)
            {
                if (BreakIfError(coapResp.Code.Value))
                {
                    return;
                }

                if (coapResp.Code.Value == CoAPMessageCode.CONTENT)
                {

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
                        if (proceed)
                        {
                            if (ccformat.Value == CoAPContentFormatOption.TEXT_PLAIN)
                            {
                                string result = AbstractByteUtils.ByteToStringUTF8(coapResp.Payload.Value);
                                __GetResult = result;
                            }
                            if (ccformat.Value == CoAPContentFormatOption.APPLICATION_OCTET_STREAM)
                            {
                                string result = HdkUtils.BytesToHexView(coapResp.Payload.Value);
                                __GetResult = result;
                            }
                            if (ccformat.Value == CoAPContentFormatOption.APPLICATION_JSON)
                            {
                                string result = HdkUtils.BytesToHexView(coapResp.Payload.Value);
                                __GetResult = result;
                            }

                            __Response = coapResp;
                            ObserveResponseHandler h = ObserveResponseReceived;
                            try
                            {
                                if (h != null)
                                {
                                    h.Invoke(coapResp);
                                }
                            }
                            catch { }
                            ////__Done.Set();
                        }
                        if(coapResp.MessageType.Value == CoAPMessageType.CON)
                        {
                            AcknowledgeResponse(coapResp);
                        }
                    }
                }
                else
                {
                    //Will come here if an error occurred..
                }
            }

        }
        private void AcknowledgeResponse(CoAPResponse coapResp)
        {
            string serverIP = __IpAddress;

            // Response handlers already set - leave them alone

            coapReq = new CoAPRequest(CoAPMessageType.ACK,
                                    CoAPMessageCode.GET,
                                    HdkUtils.MessageId());

            string uriToCall = "coap://" + HdkUtils.UriFromMac(__IpAddress) + ":" + __ServerPort + __URI;
            coapReq.SetURL(uriToCall);
            __Token = DateTime.Now.ToString("HHmmss");//Token value must be less than 8 bytes
            coapReq.Token = coapResp.Token;

            string blat = coapReq.ToString();
            // JJK - Changes in v2.0.7
            //coapReq.Options.AddOption(CoAPHeaderOption.PROXY_SCHEME, "coap");
            // make Proxy packet
            coapReq.Options.AddOption(CoAPHeaderOption.BLOCK2, new byte[] { CoAPBlockOption.BLOCK_SIZE_128 });
            coapReq.Options.AddOption(CoAPHeaderOption.PROXY_URI, "coap://" + HdkUtils.UriFromMac(__IpAddress) + ":" + "4849" + __URI);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_HOST);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_QUERY);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_PATH);
            coapReq.Options.RemoveOption(CoAPHeaderOption.URI_PORT);
            coapReq.Options.RemoveOption(CoAPHeaderOption.CONTENT_FORMAT);
            // JJK - end of Changes in v2.0.7

            FileLogger.Write("About to send CoAP ACK request");
            FileLogger.Write(coapReq.ToString());
            CoApGatewaySessionManager.Instance.Client.Send(coapReq);

        }
    }
}
