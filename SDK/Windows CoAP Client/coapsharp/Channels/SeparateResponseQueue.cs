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
using System.Threading;
using EXILANT.Labs.CoAP.Message;

namespace EXILANT.Labs.CoAP.Channels
{
    /// <summary>
    /// In CoAP, there is a concept of separate response. If the server wants sometime to process
    /// the message, it can send an empty ACK and later on, when the data is ready, it can
    /// send the response message. This is separate response. The request response is matched
    /// based on the token value. 
    /// For CON request, an ACK is sent and then later on a CON response is sent
    /// For NON request, nothing is sent, but simply after a while another NON is sent to client
    /// </summary>
    public class SeparateResponseQueue 
    {
        #region Implementation
        /// <summary>
        /// Holds requests against which a separate response is to be sent
        /// </summary>
        protected Queue _separateResponseQ = new Queue();
        /// <summary>
        /// For queue access synchronization
        /// </summary>
        protected AutoResetEvent _separateRespQSync = new AutoResetEvent(true);
        #endregion

        #region Operations
        /// <summary>
        /// Add this request to the pending separate response queue.
        /// The message can be extracted later and acted upon
        /// </summary>
        /// <param name="coapReq">CoAPRequest</param>
        public virtual void Add(CoAPRequest coapReq)
        {
            if (coapReq == null)
                throw new ArgumentNullException("CoAPRequest to add to this queue cannot be NULL");
            this._separateRespQSync.WaitOne();
            this._separateResponseQ.Enqueue(coapReq);
            this._separateRespQSync.Set();
        }
        /// <summary>
        /// Get the next request from the Q that was pending a separate response.
        /// If nothing is pending then NULL value is returned
        /// </summary>
        /// <returns>CoAPRequest</returns>
        public virtual CoAPRequest GetNextPendingRequest()
        {
            CoAPRequest coapReq = null;
            this._separateRespQSync.WaitOne();
            if (this._separateResponseQ.Count > 0 && this._separateResponseQ.Peek() != null)
                coapReq = (CoAPRequest)this._separateResponseQ.Dequeue();
            this._separateRespQSync.Set();

            return coapReq;
        }
        #endregion
    }
}
