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
using coapsharp.Properties;

using EXILANT.Labs.CoAP.Exceptions;
using EXILANT.Labs.CoAP.Message;
using EXILANT.Labs.CoAP.Helpers;
using EXILANT.Labs.CoAP.Settings;

namespace EXILANT.Labs.CoAP.Channels
{
    /// <summary>
    /// This class represents a CoAP channel that behaves like a client.
    /// This is the default implementation. If you need a different implementation,
    /// extend the AbstractCoAPChannel class and create your own
    /// </summary>
    public class CoAPClientChannel : AbstractCoAPChannel
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
        /// <summary>
        /// For thread lifetime management
        /// </summary>
        protected bool _isDone = false;

        //protected bool __ExcludePort = false;
        #endregion

        #region Constructors
        /// <summary>
        /// Default constructor
        /// </summary>
        public CoAPClientChannel()
        {
            //Setup basic parameters
            this.AckTimeout = AbstractCoAPChannel.DEFAULT_ACK_TIMEOUT_SECS;
            this.MaxRetransmissions = AbstractCoAPChannel.DEFAULT_MAX_RETRANSMIT;
        }
        #endregion

        #region Properties
        public EndPoint EndPoint
        {
            get { return _remoteEP; }
        }

        public Socket ClientSocket
        {
            get { return _clientSocket; }
        }

        //public bool ExcludePort
        //{
        //    get { return __ExcludePort; }
        //    set { __ExcludePort = true; }
        //}
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
            //ipAddr = IPAddress.IPv6Loopback;
            this._remoteEP = new IPEndPoint(ipAddr, port);
            /*Socket serverSocket =
    new Socket(ipAddress.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
                        ?

Note: There's a shortcut to obtain the IP address of localhost: You can simply use IPAddress.Loopback (127.0.0.1) or IPAddress.IPv6Loopback (::1).
*/
            this._clientSocket = new Socket(CoAPSettings.Instance.AddressFamily, SocketType.Dgram, ProtocolType.Udp);
            Console.WriteLine("NEW SOCKET - handle = " + _clientSocket.Handle.ToString());
            ////this._clientSocket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);  <==== added by Mike

            this._clientSocket.Connect(this._remoteEP);
            //Initialize ACK Q
            this._msgPendingAckQ = new TimedQueue((UInt16)this.AckTimeout);
            this._msgPendingAckQ.OnResponseTimeout += new TimedQueue.TimedOutWaitingForResponse(OnTimedOutWaitingForResponse);
            //Start message processing thread
            this._isDone = false;
            Thread waitForClientThread = new Thread(new ThreadStart(ProcessReceivedMessages));
            waitForClientThread.Start();
        }
        
        /// <summary>
        /// Shutdown the client channel
        /// </summary>
        public override void Shutdown()
        {
            this._isDone = true;
            Thread.Sleep(2000);

            if (this._clientSocket != null)
            {
                Console.WriteLine("CLOSING SOCKET - handle = " + _clientSocket.Handle.ToString());
                this._clientSocket.Close();
            }
            if (this._msgPendingAckQ != null)
                this._msgPendingAckQ.Shutdown();
            this._msgPendingAckQ = null;
            this._remoteEP = null;
        }
        #endregion

        #region Operations
        /// <summary>
        /// Send a CoAP message to the server
        /// </summary>
        /// <param name="coapMsg">The CoAP message to send to server</param>
        /// <returns>Number of bytes sent</returns>
        public override int Send(AbstractCoAPMessage coapMsg)
        {
            if (coapMsg == null) throw new ArgumentNullException("Message is NULL");
            if (this._clientSocket == null) throw new InvalidOperationException("CoAP client not yet started");
            int bytesSent = 0;
            try
            {
                byte[] coapBytes = coapMsg.ToByteStream();
                if (coapBytes.Length > AbstractNetworkUtils.GetMaxMessageSize())
                    throw new UnsupportedException("Message size too large. Not supported. Try block option");
                bytesSent = this._clientSocket.Send(coapBytes);
                if (coapMsg.MessageType.Value == CoAPMessageType.CON)
                {
                    //confirmable message...need to wait for a response
                    if (coapMsg.Timeout <= 0) coapMsg.Timeout = AbstractCoAPChannel.DEFAULT_ACK_TIMEOUT_SECS;
                    coapMsg.DispatchDateTime = DateTime.Now;
                    this._msgPendingAckQ.AddToWaitQ(coapMsg);
                }
            }
            catch (Exception e)
            {
                this._msgPendingAckQ.RemoveFromWaitQ(coapMsg.ID.Value);                
                this.HandleError(e, coapMsg);
            }
            
            return bytesSent;
        }

        public byte[] ToBytes(AbstractCoAPMessage coapMsg)
        {
            byte[] coapBytes = coapMsg.ToByteStream();
            if (coapBytes.Length > AbstractNetworkUtils.GetMaxMessageSize())
                throw new UnsupportedException("Message size too large. Not supported. Try block option");
            return coapBytes;
        }
        /// <summary>
        /// Once a confirmable message is sent, it must wait for an ACK or RST
        /// If nothing comes within the timeframe, this event is raised.
        /// </summary>
        /// <param name="coapMsg">An instance of AbstractCoAPMessage</param>
        private void OnTimedOutWaitingForResponse(AbstractCoAPMessage coapMsg)
        {
            //make an attempt to retransmit
            coapMsg.RetransmissionCount++;
            if (coapMsg.RetransmissionCount > this.MaxRetransmissions)
            {
                //Exhausted max retransmit
                this.HandleError(new UndeliveredException("Cannot deliver message. Exhausted retransmit attempts"), coapMsg);
            }
            else
            {
                coapMsg.Timeout = (int)(coapMsg.RetransmissionCount * this.AckTimeout * AbstractCoAPChannel.DEFAULT_ACK_RANDOM_FACTOR);
                //attempt resend
                this.Send(coapMsg);
            }
        }
        #endregion

        #region Thread
        /// <summary>
        /// This thread continuously looks for messages on the socket
        /// Once available, it will post them for handling downstream
        /// </summary>
        protected void ProcessReceivedMessages()
        {
            byte[] buffer = null;
            int maxSize = AbstractNetworkUtils.GetMaxMessageSize();
            while (!this._isDone)
            {
                Thread.Sleep(200);      //<==== CHANGE this to a variable and/or change the blocking mechanism.
                try
                {
                    if (this._clientSocket.Available >= 4 /*Min size of CoAP block*/)
                    {
                        buffer = new byte[maxSize * 2];
                        int bytesRead = this._clientSocket.Receive(buffer);
                        byte[] udpMsg = new byte[bytesRead];
                        Array.Copy(buffer, udpMsg, bytesRead);
                        byte mType = AbstractCoAPMessage.PeekMessageType(udpMsg);
                        
                        if ( (mType == CoAPMessageType.CON ||
                              mType == CoAPMessageType.NON) && AbstractCoAPMessage.PeekIfMessageCodeIsRequestCode(udpMsg))
                        {
                            //This is a request
                            CoAPRequest coapReq = new CoAPRequest();
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

                            this.HandleRequestReceived(coapReq);
                        }
                        else
                        {
                            //This is a response
                            CoAPResponse coapResp = new CoAPResponse();
                            coapResp.FromByteStream(udpMsg);
                            coapResp.RemoteSender = this._remoteEP;//Setup who sent this message
                            //Remove the waiting confirmable message from the timeout queue
                            if (coapResp.MessageType.Value == CoAPMessageType.ACK ||
                                coapResp.MessageType.Value == CoAPMessageType.RST)
                            {
                                this._msgPendingAckQ.RemoveFromWaitQ(coapResp.ID.Value);
                            }
                            this.HandleResponseReceived(coapResp);
                        }
                    }
                }
                catch (SocketException se)
                {
                    //Close this client connection
                    this._isDone = true;
                    this.HandleError(se, null);
                }
                catch (ArgumentNullException argEx)
                {
                    this.HandleError(argEx, null);
                }
                catch (ArgumentException argEx)
                {
                    this.HandleError(argEx, null);
                }
                catch (CoAPFormatException fEx)
                {
                    //Invalid message..
                    this.HandleError(fEx, null);
                }                
            }
        }
        #endregion
    }
}
