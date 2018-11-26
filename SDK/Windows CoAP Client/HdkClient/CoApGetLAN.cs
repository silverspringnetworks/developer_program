using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EXILANT.Labs.CoAP.Channels;
using EXILANT.Labs.CoAP.Message;
using System.Threading;
using EXILANT.Labs.CoAP.Helpers;
using EXILANT.Labs.CoAP.Settings;
using coapsharp.Properties;
using SSNUtils;
using System.Net;

namespace HdkClient
{
    public class CoApGetLAN : CoApGet
    {
        public new void Send()
        {
            string serverIP = __IpAddress;//coapsharp.Properties.Settings.Default.IpAddress;// "10.90.202.182";//"localhost";
            if (HdkUtils.IsIPv4Address(serverIP))
            {
                CoAPSettings.Instance.AddressFamily = System.Net.Sockets.AddressFamily.InterNetwork;
            }
            else
            {
                CoAPSettings.Instance.AddressFamily = System.Net.Sockets.AddressFamily.InterNetworkV6;
            }

            __coapClient.Initialize(serverIP, __ServerPort);
            __coapClient.CoAPResponseReceived += new CoAPResponseReceivedHandler(OnCoAPResponseReceived);

            __coapClient.CoAPError += new CoAPErrorHandler(OnCoAPError);

            coapReq = new CoAPRequest(this.ConfirmableMessageType,//CoAPMessageType.NON,
                                                CoAPMessageCode.GET,
                                                100);//hardcoded message ID as we are using only once
            string uriToCall = "coap://" + serverIP + ":" + __ServerPort + __URI;
            coapReq.SetURL(uriToCall);
            __Token = DateTime.Now.ToString("HHmmss");//Token value must be less than 8 bytes
            coapReq.Token = new CoAPToken(__Token);//A random token
            __coapClient.Send(coapReq);
            __Done.WaitOne();
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

        ///// <summary>
        ///// Called when a request is received...
        ///// </summary>
        ///// <param name="coapReq">The CoAPRequest object</param>
        //private void OnCoAPRequestReceived(CoAPRequest coapReq)
        //{
        //    Console.WriteLine(coapReq.QueryString);
        //}

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
                                Console.WriteLine("Get on Olimex " + __coapClient.EndPoint.ToString() + " = " + result);
                                __GetResult = result;
                            }
                            if (ccformat.Value == CoAPContentFormatOption.APPLICATION_OCTET_STREAM)
                            {
                                string result = SSNUtils.Conversion.BytesToHexView(coapResp.Payload.Value);
                                Console.WriteLine("Get on Olimex " + __coapClient.EndPoint.ToString() + " = " + result);
                                __GetResult = result;
                            }
                            if (ccformat.Value == CoAPContentFormatOption.APPLICATION_JSON)
                            {
                                string result = SSNUtils.Conversion.BytesToHexView(coapResp.Payload.Value);
                                Console.WriteLine("Get on Olimex " + __coapClient.EndPoint.ToString() + " = " + result);
                                __GetResult = result;
                            }

                            __Response = coapResp;
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
