/******************************************************************************
    CoAPSharp - C# Implementation of CoAP for .NET Micro Framework 
    Copyright [2013] [EXILANT Technologies Pvt. Ltd., www.coapsharp.com]
    
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
    Or visit http://www.gnu.org/licenses/
 *****************************************************************************/
using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

using EXILANT.Labs.CoAP.Exceptions;
using EXILANT.Labs.CoAP.Message;
using EXILANT.Labs.CoAP.Helpers;
using EXILANT.Labs.CoAP.Settings;
using coapsharp.Properties;

namespace EXILANT.Labs.CoAP.Channels
{
    /// <summary>
    /// This class represents a CoAP channel that behaves like a synchronous client.  
    /// This client does not support any event handler. All exceptions must be handled
    /// by the caller.
    /// </summary>
    public class CoAPSyncClientChannel : AbstractCoAPChannel
    {
        #region Implementation
        /// <summary>
        /// Holds the client socket
        /// </summary>
        protected Socket _clientSocket = null;
        /// <summary>
        /// Holds the remote endpoint to which this client is connected to
        /// </summary>
        protected EndPoint _remoteEP = null;        
        #endregion

        #region Constructors
        /// <summary>
        /// Default constructor
        /// </summary>
        public CoAPSyncClientChannel()
        {
            //Setup basic parameters
            this.AckTimeout = AbstractCoAPChannel.DEFAULT_ACK_TIMEOUT_SECS;
            this.MaxRetransmissions = AbstractCoAPChannel.DEFAULT_MAX_RETRANSMIT;
        }
        #endregion

        #region Lifetime Management
        /// <summary>
        /// Initialize all basic aspects of the client channel
        /// </summary>
        /// <param name="host">The IP host</param>
        /// <param name="port">The port number</param>
        public override void Initialize(string host, int port)
        {
            Shutdown();
            
            //Connect to host            
            IPAddress ipAddr = AbstractNetworkUtils.GetIPAddressFromHostname(host) ;
            this._remoteEP = new IPEndPoint(ipAddr, port);
            this._clientSocket = new Socket(CoAPSettings.Instance.AddressFamily, SocketType.Dgram, ProtocolType.Udp);
            this._clientSocket.Connect(this._remoteEP);                       
        }
        
        /// <summary>
        /// Shutdown the client channel
        /// </summary>
        public override void Shutdown()
        {
            if (this._clientSocket != null)
                this._clientSocket.Close();

            this._remoteEP = null;
        }
        #endregion

        #region Message Exchange
        /// <summary>
        /// Send a CoAP message to the server. Please note, you must handle all exceptions
        /// and no event is raised.
        /// </summary>
        /// <param name="coapMsg">The CoAP message to send to server</param>
        /// <returns>Number of bytes sent</returns>
        public override int Send(AbstractCoAPMessage coapMsg)
        {
            if (coapMsg == null) throw new ArgumentNullException("Message is NULL");
            if (this._clientSocket == null) throw new InvalidOperationException("CoAP client not yet started");
            int bytesSent = 0;
            byte[] coapBytes = coapMsg.ToByteStream();
            if (coapBytes.Length > AbstractNetworkUtils.GetMaxMessageSize())
                throw new UnsupportedException("Message size too large. Not supported. Try block option");
            bytesSent = this._clientSocket.Send(coapBytes);
            if (coapMsg.MessageType.Value == CoAPMessageType.CON)
            {
                //confirmable message...need to wait for a response
                coapMsg.DispatchDateTime = DateTime.Now;
            }
            
            return bytesSent;
        }
        /// <summary>
        /// Receive a message from the server. This will block if there
        /// are no messages. Please note, you must handle all errors (except timeout)
        /// and no error is raised.
        /// </summary>
        /// <param name="rxTimeoutMillis">
        /// The timeout value in milliseconds.The default value is 0, which indicates an infinite time-out period. 
        /// Specifying -1 also indicates an infinite time-out period
        /// </param>
        /// <param name="timedOut">Is set to true on timeout</param>
        /// <returns>An instance of AbstractCoAPMessage on success, else null on error/timeout</returns>        
        public AbstractCoAPMessage ReceiveMessage(int rxTimeoutMillis , ref bool timedOut)
        {            
            byte[] buffer = null;
            int maxSize = AbstractNetworkUtils.GetMaxMessageSize();
            CoAPRequest coapReq = null;
            CoAPResponse coapResp = null;
            try
            {
                this._clientSocket.ReceiveTimeout = rxTimeoutMillis;
                buffer = new byte[maxSize * 2];
                int bytesRead = this._clientSocket.Receive(buffer);
                byte[] udpMsg = new byte[bytesRead];
                Array.Copy(buffer, udpMsg, bytesRead);
                byte mType = AbstractCoAPMessage.PeekMessageType(udpMsg);

                if ((mType == CoAPMessageType.CON ||
                     mType == CoAPMessageType.NON) && AbstractCoAPMessage.PeekIfMessageCodeIsRequestCode(udpMsg))
                {
                    //This is a request
                    coapReq = new CoAPRequest();
                    coapReq.FromByteStream(udpMsg);
                    coapReq.RemoteSender = this._remoteEP;//Setup who sent this message
                    string uriHost = ((IPEndPoint)this._remoteEP).Address.ToString();
                    UInt16 uriPort = (UInt16)((IPEndPoint)this._remoteEP).Port;

                    //setup the default values of host and port
                    //setup the default values of host and port
                    if (!coapReq.Options.HasOption(CoAPHeaderOption.URI_HOST))
                        coapReq.Options.AddOption(CoAPHeaderOption.URI_HOST, AbstractByteUtils.StringToByteUTF8(uriHost));
                    if (!coapReq.Options.HasOption(CoAPHeaderOption.URI_PORT))
                        coapReq.Options.AddOption(CoAPHeaderOption.URI_PORT, AbstractByteUtils.GetBytes(uriPort));

                    return coapReq;
                }
                else
                {
                    //This is a response
                    coapResp = new CoAPResponse();
                    coapResp.FromByteStream(udpMsg);
                    coapResp.RemoteSender = this._remoteEP;//Setup who sent this message

                    return coapResp;
                }
            }
            catch (SocketException se)
            {
                if (se.ErrorCode == (int)SocketError.TimedOut)
                    timedOut = true;
                else
                    throw se;
            }
            return null;
        }
        #endregion
    }
}
