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
using System.Text;
using EXILANT.Labs.CoAP.Message;
using EXILANT.Labs.CoAP.Helpers;
using EXILANT.Labs.CoAP.Exceptions;

namespace EXILANT.Labs.CoAP.Message
{
    /// <summary>
    /// Holds the message ID passed around in a CoAP message
    /// </summary>
    public class CoAPMessageID : IParsable
    {
        #region Properties
        /// <summary>
        /// Accessor/Mutator for the CoAP message ID
        /// </summary>
        public UInt16 Value { get; set; }
        #endregion

        #region Constructors
        /// <summary>
        /// Default constructor
        /// </summary>
        public CoAPMessageID() { }
        /// <summary>
        /// Overloaded constructor
        /// </summary>
        /// <param name="mId">The message Id</param>
        public CoAPMessageID(UInt16 mId) { this.Value = mId; }
        #endregion

        #region Operations
        /// <summary>
        /// Parse the CoAP message stream and extract message Id (in network byte order)
        /// </summary>
        /// <param name="coapMsgStream">The CoAP message stream that contains the token length and value</param>
        /// <param name="startIndex">The index to start looking for the value</param>
        /// <param name="extraInfo">Not used</param>
        /// <returns>The next index from where next set of information can be extracted from the message stream</returns>
        public int Parse(byte[] coapMsgStream, int startIndex, UInt16 extraInfo)
        {
            if (coapMsgStream == null || coapMsgStream.Length == 0 || startIndex < 0) return startIndex;//do nothing 
            if (coapMsgStream.Length < AbstractCoAPMessage.HEADER_LENGTH) throw new CoAPFormatException("Invalid CoAP message stream");
            if (startIndex >= coapMsgStream.Length) throw new ArgumentException("Start index beyond message stream length");

            //We read two bytes...
            byte[] mid = {coapMsgStream[startIndex] ,coapMsgStream[startIndex + 1]};
            //We received them in network byte order...fix the order based on the platform
            mid = AbstractNetworkUtils.FromNetworkByteOrder(mid);
            Value = AbstractByteUtils.ToUInt16(mid);
            return (startIndex + 2);
        }
        /// <summary>
        /// Convert this object into a byte stream in network byte order
        /// </summary>
        /// <param name="reserved">Not used now</param>
        /// <returns>byte array</returns>
        public byte[] ToStream(UInt16 reserved)
        {
            byte[] mID = AbstractByteUtils.GetBytes( this.Value );
            mID = AbstractNetworkUtils.ToNetworkByteOrder(mID);
            return mID;
        }
        /// <summary>
        /// Message ID is 2 bytes...check if each of the bytes is valid (always returns true)
        /// </summary>
        /// <param name="value">The value to check</param>
        /// <returns>bool</returns>
        public bool IsValid(UInt16 value)
        {
            return true;
        }
        #endregion

        #region Overrides
        /// <summary>
        /// Convert to a string representation
        /// </summary>
        /// <returns>string</returns>
        public override string ToString()
        {
            return "ID = " + this.Value.ToString();
        }
        #endregion
    }
}
