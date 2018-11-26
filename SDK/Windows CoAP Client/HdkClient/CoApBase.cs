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
using System.Threading;

namespace HdkClient
{
    /// <summary>
    /// This is the base class for all CoAP calls of all network types.
    /// It encapsulates all the common components of a CoAP call:
    ///     - a token to match responses against
    ///     - a client channel with socket
    ///     - an object containing a wait handle for synchronizing of responses to requests
    ///     - etc.
    /// </summary>
    public abstract class CoApBase
    {
        protected CoAPClientChannel __coapClient;
        protected string __Token = "";
        protected string __IpAddress = "";
        protected int __ServerPort = 5683;
        protected int __Timeout = 5000;
        protected CoAPRequest coapReq = null;
        protected System.Threading.ManualResetEvent __Done = new ManualResetEvent(false);
        protected string __URI = "";
        protected CoAPResponse __Response = null;
        protected byte __MessageType = CoAPMessageType.NON;
        protected CoAPPayload __Payload = null;
        protected string __ErrorResult = "";
        protected bool __TimedOut = false;
        /// <summary>
        /// Default constructor.
        /// </summary>
        public CoApBase()
        {
            __coapClient = new CoAPClientChannel();
        }
        /// <summary>
        /// The CoAP response from the call.
        /// </summary>
        public CoAPResponse Response
        {
            get { return __Response; }
            set { __Response = value; }
        }
        /// <summary>
        /// The URI of the CoAP target resource.
        /// </summary>
        public string URI
        {
            get { return __URI; }
            set
            {
                __URI = value;
                if (__URI.Substring(0, 1) != "/")
                {
                    __URI = "/" + __URI;
                }
            }
        }
        /// <summary>
        /// The IP Address for the server.
        /// </summary>
        public string IpAddress
        {
            get { return __IpAddress; }
            set { __IpAddress = value; }
        }
        /// <summary>
        /// The CoAP server port to use.
        /// </summary>
        public int ServerPort
        {
            get { return __ServerPort; }
            set { __ServerPort = value; }
        }
        /// <summary>
        /// The request time out, in milliseconds.
        /// </summary>
        public int Timeout
        {
            get { return __Timeout; }
            set { __Timeout = value; }
        }
        public CoAPPayload Payload
        {
            get { return __Payload; }
            set { __Payload = value; }
        }

        /// <summary>
        /// Convert the current CoAP to a byte array.
        /// </summary>
        /// <returns></returns>
        public byte[] ToBytes()
        {
            return __coapClient.ToBytes(coapReq);
        }
        public virtual void Send()
        {
            __coapClient.Send(coapReq);

        }
        /// <summary>
        /// Return the current message type as the corresponding CoAP message type byte.
        /// </summary>
        public byte ConfirmableMessageType
        {
            get
            {
                if (CoApSettings.Instance.Confirmable)
                {
                    return CoAPMessageType.CON;
                }
                else
                {
                    return CoAPMessageType.NON;
                }
            }
        }
        /// <summary>
        /// Set a token based on a random(ish) string based on time.
        /// </summary>
        public void SetToken()
        {
            __Token = DateTime.Now.ToString("HHmmss");//Token value must be less than 8 bytes
            coapReq.Token = new CoAPToken(__Token);//A random token
        }
        /// <summary>
        /// Determine whether a response requires release from an existing wait condition.
        /// </summary>
        /// <param name="code">the response classification code, the upper-most digit of the response code</param>
        /// <returns>A boolean indicating whether or not to break.  The __Done wait handle is also triggered.</returns>
        public bool BreakIfError(byte code)
        {
            int classCode = ((code & 0xE0) >> 5);

            if (classCode == 4) // Error - possibly resource not found
            {
                __Done.Set();   // We should just stop
                return true;
            }
            if (classCode == 5) // Severe error
            {
                __Done.Set();   // We should just stop
                return true;
            }
            return false;
        }
        public string ErrorResult
        {
            get { return __ErrorResult; }
            set { __ErrorResult = value; }
        }
    }
}
