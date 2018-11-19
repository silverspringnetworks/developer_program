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

namespace EXILANT.Labs.CoAP.Message
{
    /// <summary>
    /// An object within this library, that is formed by parsing the CoAP message stream should
    /// implement this.
    /// </summary>
    public interface IParsable
    {
        #region Operations
        /// <summary>
        /// Class that represents a chunk in the CoAP message stream and that is formed by parsing
        /// the message stream, should implement this
        /// </summary>
        /// <param name="coapMsgStream">The CoAP message stream that is to be parsed</param>
        /// <param name="startIndex">The index from where to begin parsing</param>
        /// <param name="extraInfo">Any additional information</param>
        /// <returns>The next position in the stream from where others should start looking for other values</returns>
        int Parse(byte[] coapMsgStream, int startIndex, UInt16 extraInfo);
        /// <summary>
        /// Convert the object to a stream of bytes
        /// </summary>
        /// <param name="extraInfo">unsigned int 16 bits to pass any additional information for stream conversion</param>
        /// <returns>byte stream</returns>
        byte[] ToStream(UInt16 extraInfo);
        /// <summary>
        /// Check if the value is valid or not
        /// </summary>
        /// <param name="value">The value to check for validity</param>
        /// <returns>boolean</returns>
        bool IsValid(UInt16 value);
        #endregion
    }
}
