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
using EXILANT.Labs.CoAP.Helpers;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using EXILANT.Labs.CoAP.Message;
using EXILANT.Labs.CoAP.Settings;
using System.Collections;
using coapsharp.Properties;

using EXILANT.Labs.CoAP.Exceptions;

namespace EXILANT.Labs.CoAP.Channels
{
    /// <summary>
    /// This class implements a server node type. Any constrained device, that is
    /// willing to act as a server, shoul make use of this class
    /// </summary>
    public class CoAPServerChannel : AbstractCoAPChannel
    {
        #region Implementation
        /// <summary>
        /// The underlying socket
        /// </summary>
        protected Socket _socket = null;
        /// <summary>
        /// For thread lifetime management
        /// </summary>
        protected bool _isDone = false;
        /// <summary>
        /// Holds the port number to listen on
        /// </summary>
        protected int _port = 0;
        /// <summary>
        /// Holds the hostname/ip address
        /// </summary>
        protected string _host = null;
        /// <summary>
        /// Holds messages pending separate response
        /// </summary>
        protected SeparateResponseQueue _separateResponseQ = null;
        /// <summary>
        /// Holds a list of observers
        /// </summary>
        protected ObserversList _observers = null;
        #endregion                

        #region Properties
        /// <summary>
        /// Accessor for the list of clients that are currently 
        /// observing one or more resources on this server
        /// </summary>
        public ObserversList ObserversList { get { return this._observers; } }
        #endregion

        #region Constructors
        /// <summary>
        /// Default constructor
        /// </summary>
        public CoAPServerChannel()
        {
            //Setup basic parameters
            this.AckTimeout = AbstractCoAPChannel.DEFAULT_ACK_TIMEOUT_SECS;
            this.MaxRetransmissions = AbstractCoAPChannel.DEFAULT_MAX_RETRANSMIT;
        }
        #endregion

        #region Server Management
        /// <summary>
        /// Start the server
        /// </summary>
        /// <param name="host">Hostname to use for this server (DNS/IP)</param>
        /// <param name="port">The port number to listen on</param>
        public override void Initialize(string host, int port)        
        {
            this._host = (host == null || host.Trim().Length ==0 ) ? "unknown" : host;
            if (port <= 0)
                this._port = AbstractNetworkUtils.GetDefaultCoAPPort();
            else
                this._port = port;
            
            Shutdown(); //close all previous connections
            
            //Create the wait q
            this._msgPendingAckQ = new TimedQueue((uint)AbstractCoAPChannel.DEFAULT_ACK_TIMEOUT_SECS);
            this._msgPendingAckQ.OnResponseTimeout += new TimedQueue.TimedOutWaitingForResponse(OnTimedOutWaitingForResponse);
            
            //Create the separate response q
            this._separateResponseQ = new SeparateResponseQueue();

            //Create the observers list
            this._observers = new ObserversList();

            // Create a socket, bind it to the server's port and listen for client connections            
            this._isDone = false;
            this.ReInitSocket();
            Thread waitForClientThread = new Thread(new ThreadStart(WaitForConnections));
            waitForClientThread.Start();
        }        
        /// <summary>
        /// Re-initialize the socket only
        /// </summary>
        /// <returns>bool (true on success else false)</returns>
        protected bool ReInitSocket()
        {
            bool success = false;
            try
            {
                if (this._socket != null) this._socket.Close();
                this._socket = null;
                this._socket = new Socket(CoAPSettings.Instance.AddressFamily, SocketType.Dgram, ProtocolType.Udp); /*CoAP is UDP based*/
                EndPoint localEndPoint = new IPEndPoint(IPAddress.IPv6Any, this._port);
                this._socket.Bind(localEndPoint);   
                success = true;
            }
            catch (Exception e)
            {
                this.HandleError(e, null);
            }
            return success;
        }
        /// <summary>
        /// Shutdown the client channel
        /// </summary>
        public override void Shutdown()
        {
            this._isDone = true;
            if (this._socket != null)
            {
                this._socket.Close();
                this._socket = null;
            }
            if (this._msgPendingAckQ != null)
                this._msgPendingAckQ.Shutdown();
            this._msgPendingAckQ = null;            
            this._separateResponseQ = null;
            this._observers = null;
        }
        #endregion

        #region Message Sending
        /// <summary>
        /// Send the CoAP message to client
        /// </summary>
        /// <param name="coapMsg">The CoAP message to send</param>
        /// <returns>Number of bytes sent</returns>
        public override int Send(AbstractCoAPMessage coapMsg)
        {
            if (coapMsg == null) throw new ArgumentNullException("Message is NULL");            
            if (this._socket == null) throw new InvalidOperationException("CoAP server not yet started");

            int bytesSent = 0;
            try
            {
                //We do not want server to die when a socket send error occurs...
                //just clean all settings specific to the remote client and
                //keep going                
                byte[] coapBytes = coapMsg.ToByteStream();
                bytesSent = this._socket.SendTo(coapBytes, coapMsg.RemoteSender);
                if (coapMsg.MessageType.Value == CoAPMessageType.CON)
                {
                    //confirmable message...need to wait for a response
                    coapMsg.DispatchDateTime = DateTime.Now;
                    this._msgPendingAckQ.AddToWaitQ(coapMsg);
                }
            }
            catch(Exception e)
            {
                this._msgPendingAckQ.RemoveFromWaitQ(coapMsg.ID.Value);
                if (coapMsg.GetType() == typeof(CoAPRequest))
                {
                    CoAPRequest coapReq = (CoAPRequest)coapMsg;
                    this._observers.RemoveResourceObserver(coapReq.GetURL() , coapReq.Token.Value);
                }
                else if (coapMsg.GetType() == typeof(CoAPResponse))
                {
                    CoAPResponse coapResp = (CoAPResponse)coapMsg;
                    this._observers.RemoveResourceObserver(coapResp);
                }
                this.HandleError(e, coapMsg);
            }
            
            return bytesSent;
        }
        /// <summary>
        /// Once a confirmable message is sent, it must wait for an ACK or RST
        /// If nothing comes within the timeframe, this event is raised.
        /// </summary>
        /// <param name="coapMsg">An instance of AbstractCoAPMessage</param>
        protected virtual void OnTimedOutWaitingForResponse(AbstractCoAPMessage coapMsg)
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
                coapMsg.Timeout = (int)(Math.Pow(AbstractCoAPChannel.DEFAULT_ACK_TIMEOUT_SECS, coapMsg.RetransmissionCount + 1) * AbstractCoAPChannel.DEFAULT_ACK_RANDOM_FACTOR);
                //attempt resend
                this.Send(coapMsg);
            }
        }
        /// <summary>
        /// Add this request to the pending separate response queue.
        /// The message can be extracted later and acted upon
        /// </summary>
        /// <param name="coapReq">CoAPRequest</param>
        public virtual void AddToPendingSeparateResponse(CoAPRequest coapReq)
        {
            if (this._separateResponseQ == null)
                throw new InvalidOperationException("Please initialize the server first");
            if (coapReq == null)
                throw new ArgumentNullException("CoAPRequest to add to this queue cannot be NULL");            
            this._separateResponseQ.Add(coapReq);
        }
        /// <summary>
        /// Get the next request from the Q that was pending a separate response.
        /// If nothing is pending then NULL value is returned
        /// </summary>
        /// <returns>CoAPRequest</returns>
        public virtual CoAPRequest GetNextRequestPendingSeparateResponse()
        {
            if (this._separateResponseQ == null)
                throw new InvalidOperationException("Please initialize the server first");
            CoAPRequest coapReq = (CoAPRequest)this._separateResponseQ.GetNextPendingRequest();
            return coapReq;
        }
        #endregion

        #region Socket Thread
        /// <summary>
        /// This is the thread where the socket server will accept client connections and process
        /// </summary>
        protected void WaitForConnections()
        {
            EndPoint sender = null;
            byte[] buffer = null;
            ArrayList previousBytes = new ArrayList();
            int bytesRead = 0;
            byte mType = 0;
            UInt16 mId = 0;
            byte[] udpMsg = null;
            int maxSize = AbstractNetworkUtils.GetMaxMessageSize();
            while (!this._isDone)
            {
                try
                {
                    if (this._socket.Available >= 4 /*Min size of CoAP block*/)
                    {
                        sender = new IPEndPoint(IPAddress.IPv6Any, 0);
                        buffer = new byte[maxSize * 2];
                        bytesRead = this._socket.ReceiveFrom(buffer, ref sender);
                        udpMsg = new byte[bytesRead];
                        Array.Copy(buffer, udpMsg, bytesRead);
                        
                        mType = AbstractCoAPMessage.PeekMessageType(udpMsg);
                        mId = AbstractCoAPMessage.PeekMessageID(udpMsg);

                        if ((mType == CoAPMessageType.CON ||
                              mType == CoAPMessageType.NON) && AbstractCoAPMessage.PeekIfMessageCodeIsRequestCode(udpMsg))
                            this.ProcessRequestMessageReceived(udpMsg, ref sender);
                        else
                            this.ProcessResponseMessageReceived(udpMsg, ref sender);
                    }
                    else
                    {
                        //Nothing on the socket...wait
                        Thread.Sleep(5000);
                    }
                }
                catch (SocketException se)
                {
                    //Try to re-initialize socket, and proceed only when the socket
                    //is successfully re-initialized
                    this._isDone = !this.ReInitSocket();
                    this.HandleError(se, null);
                }
                catch (ArgumentNullException argEx)
                {
                    if (mType == CoAPMessageType.CON)
                        this.RespondToBadCONRequest(mId);
                    this.HandleError(argEx, null);
                }
                catch (ArgumentException argEx)
                {
                    if (mType == CoAPMessageType.CON)
                        this.RespondToBadCONRequest(mId);
                    this.HandleError(argEx, null);
                }
                catch (CoAPFormatException fEx)
                {
                    //Invalid message..
                    if (mType == CoAPMessageType.CON)
                        this.RespondToBadCONRequest(mId);
                    this.HandleError(fEx, null);
                }
                Console.WriteLine("looping");                             
            }
            Console.WriteLine("_IsDone");
        }
        #endregion

        #region Helpers
        /// <summary>
        /// Handle request received from remote clients
        /// </summary>
        /// <param name="msgStream">The byte stream that represents a request message</param>
        /// <param name="sender">The remote sender endpoint</param>
        protected virtual void ProcessRequestMessageReceived(byte[] msgStream , ref EndPoint sender)
        {            
            CoAPRequest coapReq = new CoAPRequest();
            IPEndPoint remoteSender = (IPEndPoint) sender;
            try
            {
                coapReq.FromByteStream(msgStream);
                coapReq.RemoteSender = new IPEndPoint(remoteSender.Address , remoteSender.Port);//Setup who sent this message
                
                //setup the default values of host and port
                if (!coapReq.Options.HasOption(CoAPHeaderOption.URI_HOST))
                    coapReq.Options.AddOption(CoAPHeaderOption.URI_HOST, AbstractByteUtils.StringToByteUTF8(this._host));
                if (!coapReq.Options.HasOption(CoAPHeaderOption.URI_PORT))
                    coapReq.Options.AddOption(CoAPHeaderOption.URI_PORT, AbstractByteUtils.GetBytes((UInt16)this._port));

                if (coapReq.MessageType.Value == CoAPMessageType.CON &&
                    coapReq.Code.Value == CoAPMessageCode.EMPTY)
                {
                    //This is a PING..send a RST
                    this.RespondToPing(coapReq);
                }
                else
                {                    
                    this.HandleRequestReceived(coapReq);//Other messages, let program handle it
                }
            }
            catch
            {
                ;//TOCHECK::Do nothing, we do not want to crash the server just because we
                //could not process one received message...will check later how to improve this
            }            
        }
        /// <summary>
        /// Handle response message received from remote clients
        /// </summary>
        /// <param name="msgStream">The byte stream that represents a response message</param>
        /// <param name="sender">The remote sender endpoint</param>
        protected virtual void ProcessResponseMessageReceived(byte[] msgStream, ref EndPoint sender)
        {
            CoAPResponse coapResp = new CoAPResponse();
            IPEndPoint remoteSender = (IPEndPoint)sender;
            try
            {
                //This is a response                
                coapResp.FromByteStream(msgStream);
                coapResp.RemoteSender = new IPEndPoint(remoteSender.Address, remoteSender.Port);//Setup who sent this message
                //Remove the waiting confirmable message from the timeout queue
                if (coapResp.MessageType.Value == CoAPMessageType.RST ||
                    coapResp.MessageType.Value == CoAPMessageType.ACK)
                    this._msgPendingAckQ.RemoveFromWaitQ(coapResp.ID.Value);
                //If this is a RST, remove any observers
                if (coapResp.MessageType.Value == CoAPMessageType.RST)
                    this._observers.RemoveResourceObserver(coapResp);
                this.HandleResponseReceived(coapResp);
            }
            catch
            {
                ;//TOCHECK::Do nothing, we do not want to crash the server just because we
                //could not process one received message...will check later how to improve this
            }
        }
        /// <summary>
        /// When a CON is received with EMPTY message type from the client, it means the
        /// client is simply pinging. We need to send a RST message
        /// </summary>
        /// <param name="pingReq">The ping request</param>
        protected virtual void RespondToPing(CoAPRequest pingReq)
        {
            CoAPResponse resp = new CoAPResponse(CoAPMessageType.RST, CoAPMessageCode.EMPTY, pingReq);
            this.Send(resp);
        }
        /// <summary>
        /// When a CON is received but we cannot understand the message any further for any reason
        /// (e.g. invalid format), we want to send a RST. We make a best case attempt to
        /// find out the message ID if possible, else we send zero
        /// </summary>
        /// <param name="mId">The message Id</param>
        protected virtual void RespondToBadCONRequest(UInt16 mId)
        {
            CoAPResponse resp = new CoAPResponse(CoAPMessageType.RST, CoAPMessageCode.BAD_REQUEST , mId);            
            this.Send(resp);
        }
        #endregion
    }
}
