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

using System.Collections;
using EXILANT.Labs.CoAP.Message;
using System.Threading;
using EXILANT.Labs.CoAP.Helpers;

namespace EXILANT.Labs.CoAP.Channels
{
    /// <summary>
    /// In CoAP, confirmable messages must wait for an ACK or RST. As long as we do not
    /// receive ACK or RST we continue to send the confirmable message, provided, we have
    /// not exhausted the maximum number of retries.
    /// This queue maintains a list of confirmable messages that are waiting for ACK or RST
    /// If we do not receive the response within the timeout, this queue raises a timed-out
    /// event
    /// </summary>
    public class TimedQueue
    {
        #region Implementation
        /// <summary>
        /// Holds all the confirmable messages waiting for ACK or RST
        /// </summary>
        protected ArrayList _conMessageQ = new ArrayList();
        /// <summary>
        /// The minimum timeout in seconds
        /// </summary>
        protected uint _minTimeoutSecs = 2;
        /// <summary>
        /// For thread lifetime management
        /// </summary>
        protected bool _isDone = false;
        /// <summary>
        /// For thread synchronization
        /// </summary>
        protected AutoResetEvent _waitEvent = null;
        #endregion

        #region Delegates/Events
        /// <summary>
        /// Delegate the defines the signature of the event that will be raised when
        /// a confirmable message times out waiting for a response
        /// </summary>
        /// <param name="coapMsg">AbstractCoAPMessage of type CON</param>
        public delegate void TimedOutWaitingForResponse(AbstractCoAPMessage coapMsg);
        /// <summary>
        /// The event that is raised when a confirmable message times out waiting for a response.
        /// </summary>
        public event TimedOutWaitingForResponse OnResponseTimeout;
        #endregion

        #region Constructors
        /// <summary>
        /// We start a thread here that keeps polling the confirmable messages
        /// It checks if the message timed-out, and if yes, raises the timeout event
        /// </summary>
        /// <param name="minTimeoutSecs">What is the minimum timeout in seconds</param>
        public TimedQueue(uint minTimeoutSecs)
        {
            if (minTimeoutSecs < 1) throw new ArgumentException("Minimum timeout in seconds cannot be less than 1");
            this._waitEvent = new AutoResetEvent(true);
            Thread pollThread = new Thread(new ThreadStart(PollQ));
            pollThread.Start();
        }
        #endregion

        #region Operations
        /// <summary>
        /// Each CON/NON message is associated with a message ID.
        /// CON messages cannot be discarded unless ACK/RST is received
        /// or delivery attempt fails. This method tells us which message
        /// IDs are associated with pending CON messages
        /// </summary>
        /// <returns>ArrayList</returns>
        public ArrayList GetInUseMessageIDs()
        {
            this._waitEvent.WaitOne();//Wait for thread to release the queue
            ArrayList inUseMsgIDs = new ArrayList();
            foreach (AbstractCoAPMessage coapMsg in this._conMessageQ)
                inUseMsgIDs.Add(coapMsg.ID.Value);
            this._waitEvent.Set();//Now allow others to work on the queue
            return inUseMsgIDs;
        }
        /// <summary>
        /// Add message to wait queue
        /// </summary>
        /// <param name="coapMsg">AbstractCoAPMessage</param>
        public void AddToWaitQ(AbstractCoAPMessage coapMsg)
        {
            if (coapMsg == null)
                throw new ArgumentNullException("CoAP message cannot be null and must be of type CON");
            if (coapMsg.MessageType.Value != CoAPMessageType.CON)
                throw new ArgumentException("CoAP message to be added to wait queue must be of type CON");
            this._waitEvent.WaitOne();//Wait for thread to release the queue
            this._conMessageQ.Add(coapMsg);
            this._waitEvent.Set();//Now allow others to work on the queue
        }
        /// <summary>
        /// Remove message from wait queue
        /// </summary>
        /// <param name="msgId">The message Id corresponding to the message that needs to be removed</param>
        public void RemoveFromWaitQ(UInt16 msgId)
        {
            bool found = false;
            this._waitEvent.WaitOne();//Wait for thread to release the queue            
            try
            {
                int count = 0;
                for (count = 0; count < this._conMessageQ.Count; count++)
                {
                    AbstractCoAPMessage coapMsgInQ = (AbstractCoAPMessage)this._conMessageQ[count];
                    if (coapMsgInQ.ID.Value == msgId)
                    {
                        found = true;
                        break;
                    }
                }
                if(found && count < this._conMessageQ.Count)
                    this._conMessageQ.RemoveAt(count);
            }
            catch (Exception e)
            {
                AbstractLogUtil.GetLogger().LogError(e.ToString());
                ; ;//TOCHECK::nothing for now
            }
            finally
            {
                this._waitEvent.Set();//Now allow others to work on the queue
            }            
        }
        /// <summary>
        /// Get the request message that was pending response
        /// </summary>
        /// <param name="msgId">The message Id corresponding to the message that needs to be extracted</param>
        /// <returns>A copy of the request object pending response (if found), else null</returns>
        public CoAPRequest GetRequestPendingResponse(UInt16 msgId)
        {                        
            this._waitEvent.WaitOne();//Wait for thread to release the queue            
            CoAPRequest foundReq = null;
            try
            {
                int count = 0;
                for (count = 0; count < this._conMessageQ.Count; count++)
                {
                    AbstractCoAPMessage coapMsgInQ = (AbstractCoAPMessage)this._conMessageQ[count];
                    if (coapMsgInQ.ID.Value == msgId)
                    {
                        /*Found*/
                        foundReq = new CoAPRequest(coapMsgInQ.MessageType.Value, coapMsgInQ.Code.Value, coapMsgInQ.ID.Value);
                        foreach (CoAPHeaderOption option in coapMsgInQ.Options)
                            foundReq.AddOption(option.Number, option.Value);
                        foundReq.Token = coapMsgInQ.Token;
                        foundReq.Payload = coapMsgInQ.Payload;
                        foundReq.RemoteSender = coapMsgInQ.RemoteSender;
                        break;
                    }
                }                
            }
            catch (Exception e)
            {
                AbstractLogUtil.GetLogger().LogError(e.ToString());
                ; ;//TOCHECK::nothing for now
            }
            finally
            {
                this._waitEvent.Set();//Now allow others to work on the queue
            }
            return foundReq;
        }
        /// <summary>
        /// Shutdown the polling
        /// </summary>
        public void Shutdown()
        {
            this._isDone = true;
            this._waitEvent.Set();
        }
        #endregion

        #region Thread
        /// <summary>
        /// The thread procedure that continues to poll
        /// the queue for timeout
        /// </summary>
        protected void PollQ()
        {
            bool timedOut = false;
            int count = 0;
            while (!this._isDone)
            {
                Thread.Sleep((int)this._minTimeoutSecs * 1000);
                _waitEvent.WaitOne();//Wait for others to release the Q
                for (count = 0; count < this._conMessageQ.Count; count++)
                {
                    AbstractCoAPMessage coapMsg = (AbstractCoAPMessage)this._conMessageQ[count];
                    DateTime timeoutAt = coapMsg.DispatchDateTime.AddSeconds(coapMsg.Timeout);
                    if (timeoutAt < DateTime.Now /*Message timed out*/)
                    {
                        timedOut = true;
                        break;//We process one timeout at a time
                    }
                }
                if (timedOut && count < this._conMessageQ.Count)
                {
                    AbstractCoAPMessage coapMsg = (AbstractCoAPMessage)this._conMessageQ[count];
                    this._conMessageQ.RemoveAt(count);
                    this._waitEvent.Set();//Release Q
                    count = 0;
                    timedOut = false;

                    if (this.OnResponseTimeout != null)
                    {
                        try { this.OnResponseTimeout(coapMsg); }
                        catch (Exception e) { AbstractLogUtil.GetLogger().LogError(e.ToString()); /*we do not want to crash the Q because client crashed processing*/}
                    }
                }
                this._waitEvent.Set();//Release Q...for all cases
            }
        }
        #endregion
    }
}
