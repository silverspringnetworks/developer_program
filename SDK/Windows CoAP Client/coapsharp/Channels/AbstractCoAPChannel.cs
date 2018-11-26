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
using System.Collections;

using EXILANT.Labs.CoAP.Message;
using EXILANT.Labs.CoAP.Helpers;

namespace EXILANT.Labs.CoAP.Channels
{
    #region delegates
    /// <summary>
    /// Delegate to handle events when a CoAP request is received by the server
    /// </summary>
    /// <param name="coapReq">CoAPRequest</param>
    public delegate void CoAPRequestReceivedHandler(CoAPRequest coapReq);
    /// <summary>
    /// Delegate to handle events when a CoAP response is received by the server
    /// </summary>
    /// <param name="coapResp">CoAPResponse</param>
    public delegate void CoAPResponseReceivedHandler(CoAPResponse coapResp);
    /// <summary>
    /// Delegate to handle events when an error happens in processing
    /// </summary>
    /// <param name="e">Exception object</param>
    /// <param name="associatedMsg">The associated message with which the exception occurred</param>
    public delegate void CoAPErrorHandler(Exception e, AbstractCoAPMessage associatedMsg);
    #endregion
    /// <summary>
    /// A CoAP communication channel can act like a server or like a client.
    /// Either implementation must extend this class
    /// </summary>
    public abstract class AbstractCoAPChannel
    {
        #region Constants
        /// <summary>
        /// The default value for ACK timeout (in secs)
        /// </summary>
        public const int DEFAULT_ACK_TIMEOUT_SECS = 2;
        /// <summary>
        /// The default ACK value randomization factor
        /// </summary>
        public const float DEFAULT_ACK_RANDOM_FACTOR = 1.5F;
        /// <summary>
        /// The default maximum re-transmission attempts
        /// </summary>
        public const int DEFAULT_MAX_RETRANSMIT = 4;
        /// <summary>
        /// How many simultaneous outstanding transactions should be maintained by this endpoint
        /// </summary>
        public const int DEFAULT_NSTART = 1;
        /// <summary>
        /// The default leisure in seconds to respond to a multicast request
        /// </summary>
        public const int DEFAULT_LEISURE_SECS = 5;
        /// <summary>
        /// Default average data rate to probe a non-responding node
        /// </summary>
        public const int DEFAULT_PROBING_RATE_BYTES_PER_SEC = 1;
        /// <summary>
        /// The maximum latency period in seconds. The amount of time the message takes
        /// to reach the destination.
        /// </summary>
        public const int MAX_LATENCY_SECS = 100;
        #endregion

        #region Implementation
        /// <summary>
        /// The queue that holds all messages that are pending an ACK/RST
        /// </summary>
        protected TimedQueue _msgPendingAckQ = null;
        /// <summary>
        /// The global message Id holder
        /// </summary>
        protected UInt16 _gmsgId = 0;
        #endregion

        #region Events
        /// <summary>
        /// The event that is raised when a CoAP request is received by the server
        /// </summary>
        public event CoAPRequestReceivedHandler CoAPRequestReceived;
        /// <summary>
        /// The event that is raised when a CoAP response is received by the server
        /// </summary>
        public event CoAPResponseReceivedHandler CoAPResponseReceived;
        /// <summary>
        /// The event that is raised when an error happens during processing
        /// </summary>
        public event CoAPErrorHandler CoAPError;
        #endregion

        #region Properties
        /// <summary>
        /// Accessor/Mutator for ACK timeout value in seconds
        /// </summary>
        public int AckTimeout { get; set; }
        /// <summary>
        /// Accessor/Mutator for maximum retransmissions
        /// </summary>
        public int MaxRetransmissions { get; set; }
        /// <summary>
        /// Accessor for the maximum time between first transmission of a CON request
        /// and the last re-transmission
        /// </summary>
        public int MaxTransmitSpan { get { return (int)(AckTimeout * ((2 ^ MaxRetransmissions) - 1) * AbstractCoAPChannel.DEFAULT_ACK_RANDOM_FACTOR); } }
        /// <summary>
        /// Accessor for the maximum time starting from first transmission and ending when
        /// we giveup sending any more attempts
        /// </summary>
        public int MaxTransmitWait { get { return (int)(AckTimeout * ((2 ^ (MaxRetransmissions + 1)) - 1) * AbstractCoAPChannel.DEFAULT_ACK_RANDOM_FACTOR); } }
        /// <summary>
        /// Accessor to indicate the maximum delay a node will induce to process the received message
        /// </summary>
        public int ProcessingDelay { get { return AckTimeout; } }
        /// <summary>
        /// Accessor to get the maximum amount of time a confirmable message waits after sending the message
        /// to decide whether to drop attempts or not. This is nothing but transmit span period with 
        /// added considerations for latency and processing delay
        /// </summary>
        public int ExchangeLifetime { get { return (MaxTransmitSpan + ProcessingDelay + 2 * (AbstractCoAPChannel.MAX_LATENCY_SECS)); } }
        /// <summary>
        /// Accessor to get the maximum duration a non-confirmable message can take for transmission.
        /// After this time, the message id within the non-confirmable message can be reused
        /// </summary>
        public int NonLifetime { get { return (int)(MaxTransmitSpan + AbstractCoAPChannel.MAX_LATENCY_SECS); } }
        #endregion

        #region Abstract Methods
        /// <summary>
        /// Initialize the channel
        /// </summary>
        /// <param name="host">The IP host</param>
        /// <param name="port">The port number</param>
        public abstract void Initialize(string host, int port);
        /// <summary>
        /// Shutdown the channel
        /// </summary>
        public abstract void Shutdown();
        /// <summary>
        /// Send a message
        /// </summary>
        /// <param name="coapMsg">The CoAP message to send</param>
        /// <returns>The number of bytes sent</returns>
        public abstract int Send(AbstractCoAPMessage coapMsg);
        #endregion

        #region Event Handlers
        /// <summary>
        /// Raised when a CoAP request is received
        /// </summary>
        /// <param name="coapReq">CoAPRequest</param>
        protected void HandleRequestReceived(CoAPRequest coapReq)
        {
            CoAPRequestReceivedHandler reqRxHandler = CoAPRequestReceived; 
            try
            {
                if (reqRxHandler != null) reqRxHandler(coapReq);
            }
            catch (Exception e)
            {
                AbstractLogUtil.GetLogger().LogError(e.ToString());
                //Do nothing else...do not want to bring down the whole thing because handler failed
            }
        }
        /// <summary>
        /// Raised when a CoAP response is received
        /// </summary>
        /// <param name="coapResp">CoAPResponse</param>
        protected void HandleResponseReceived(CoAPResponse coapResp)
        {
            CoAPResponseReceivedHandler respRxHandler = CoAPResponseReceived;
            try
            {
                if (respRxHandler != null) respRxHandler(coapResp);
            }
            catch (Exception e)
            {
                AbstractLogUtil.GetLogger().LogError(e.ToString());
                //Do nothing else...do not want to bring down the whole thing because handler failed
            }
        }
        /// <summary>
        /// Handle error conditions during CoAP exchange
        /// </summary>
        /// <param name="ex">The exception that occurred</param>
        /// <param name="coapMsg">The CoAP message</param>
        protected void HandleError(Exception ex, AbstractCoAPMessage coapMsg)
        {
            CoAPErrorHandler errHandler = CoAPError;
            try
            {
                if (errHandler != null) errHandler(ex, coapMsg);
            }
            catch (Exception e)
            {
                AbstractLogUtil.GetLogger().LogError(e.ToString());
                //Do nothing else...do not want to bring down the whole thing because handler failed
            }
        }
        #endregion        

        #region Helpers
        /// <summary>
        /// Get the next message ID. We increment the global message Id
        /// check in the pending ACK queue for in-use message IDs and then
        /// return the next available one
        /// </summary>
        /// <returns>UInt16</returns>
        public virtual UInt16 GetNextMessageID()
        {
            ArrayList inUseMsgIDs = this._msgPendingAckQ.GetInUseMessageIDs();
            this._gmsgId++;
            while (inUseMsgIDs.Contains(this._gmsgId)) this._gmsgId++;//TOCHECK::Rethink
            return this._gmsgId;
        }
        #endregion
    }
}
