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
using EXILANT.Labs.CoAP.Exceptions;

namespace EXILANT.Labs.CoAP.Message
{    
    /// <summary>
    /// Represents the message codes (8-bit in 3.5 format) associated with the message
    /// </summary>
    public class CoAPMessageCode : IParsable
    {
        #region Constants
        #region Method Codes
        /// <summary>
        /// The GET method code
        /// </summary>
        public const byte GET = 0x01;
        /// <summary>
        /// The POST method code
        /// </summary>
        public const byte POST = 0x02;
        /// <summary>
        /// The PUT method code
        /// </summary>
        public const byte PUT = 0x03;
        /// <summary>
        /// The DELETE method code
        /// </summary>
        public const byte DELETE = 0x04;
        #endregion

        #region Response Codes
        /// <summary>
        /// The EMPTY request/response code
        /// </summary>
        public const byte EMPTY = 0x00;
        /// <summary>
        /// The CREATED response code
        /// </summary>
        public const byte CREATED = 0x41;
        /// <summary>
        /// The DELETED response code
        /// </summary>
        public const byte DELETED = 0x42;
        /// <summary>
        /// The VALID response code
        /// </summary>
        public const byte VALID = 0x43;
        /// <summary>
        /// The CHANGED response code
        /// </summary>
        public const byte CHANGED = 0x44;
        /// <summary>
        /// The CONTENT response code
        /// </summary>
        public const byte CONTENT = 0x45;
        /// <summary>
        /// The BAD REQUEST response code
        /// </summary>
        public const byte BAD_REQUEST = 0x80;
        /// <summary>
        /// The UNAUTHORIZED response code
        /// </summary>
        public const byte UNAUTHORIZED = 0x81;
        /// <summary>
        /// The BAD OPTION response code
        /// </summary>
        public const byte BAD_OPTION = 0x82;
        /// <summary>
        /// The FORBIDDEN response code
        /// </summary>
        public const byte FORBIDDEN = 0x83;
        /// <summary>
        /// The NOT FOUND response code
        /// </summary>
        public const byte NOT_FOUND = 0x84;
        /// <summary>
        /// The METHOD NOT ALLOWED response code
        /// </summary>
        public const byte METHOD_NOT_ALLOWED = 0x85;
        /// <summary>
        /// The NOT ACCEPTABLE response code
        /// </summary>
        public const byte NOT_ACCEPTABLE = 0x86;
        /// <summary>
        /// The REQUEST ENTITY INCOMPLETE response code (for block trasnfers)
        /// </summary>
        public const byte REQUEST_ENTITY_INCOMPLETE = 0x88;
        /// <summary>
        /// The PRECONDITION FAILED response code
        /// </summary>
        public const byte PRECONDITION_FAILED = 0x8C;
        /// <summary>
        /// The REQUEST ENTITY TOO LARGE response code
        /// </summary>
        public const byte REQUEST_ENTITY_TOO_LARGE = 0x8D;
        /// <summary>
        /// The UNSUPPORTED CONTENT FORMAT response code
        /// </summary>
        public const byte UNSUPPORTED_CONTENT_FORMAT = 0x8F;
        /// <summary>
        /// The INTERNAL SERVER ERROR response code
        /// </summary>
        public const byte INTERNAL_SERVER_ERROR = 0xA0;
        /// <summary>
        /// The NOT IMPLEMENTED response code
        /// </summary>
        public const byte NOT_IMPLEMENTED = 0xA1;
        /// <summary>
        /// The BAD GATEWAY response code
        /// </summary>
        public const byte BAD_GATEWAY = 0xA2;
        /// <summary>
        /// The SERVICE UNAVAILABLE response code
        /// </summary>
        public const byte SERVICE_UNAVAILABLE = 0xA3;
        /// <summary>
        /// The GATEWAY TIMEOUT response code
        /// </summary>
        public const byte GATEWAY_TIMEOUT = 0xA4;
        /// <summary>
        /// The PROXYING NOT SUPPORTED response code
        /// </summary>
        public const byte PROXYING_NOT_SUPPORTED = 0xA5;
        /// <summary>
        /// CONTINUE...indicates that the transfer of this
        /// block of the request body was successful and that the serve
        /// encourages sending further blocks (added in core-block-14 specs)
        /// </summary>
        public const byte CONTINUE = 0x5F;
        #endregion
        #endregion

        #region Implementation
        /// <summary>
        /// Holds the message code
        /// </summary>
        protected byte _mCode = 0;
        #endregion

        #region Properties
        /// <summary>
        /// Accessor/Mutator for the message code value
        /// </summary>
        public byte Value 
        {
            get { return _mCode; }
            set
            {
                if (!this.IsValid(value)) throw new CoAPFormatException("Invalid message code");
                this._mCode = value;
            }
        }
        #endregion

        #region Constructors
        /// <summary>
        /// Default constructor
        /// </summary>
        public CoAPMessageCode() { }
        /// <summary>
        /// Overloaded constructor
        /// </summary>
        /// <param name="mCode">The message code</param>
        public CoAPMessageCode(byte mCode)
        {
            if (!this.IsValid(mCode)) throw new ArgumentException("Invalid message code");
            this._mCode = mCode;
        }
        #endregion

        #region Operations
        /// <summary>
        /// Check if the given message code represents a request
        /// A request can only contain GET, POST, PUT, DELETE message codes
        /// EMPTY message code is a special case (for ping requests)
        /// </summary>
        /// <param name="mCode">The message code</param>
        /// <returns>boolean</returns>
        public static bool DoesMessageCodeRepresentARequest(byte mCode)
        {
            return (mCode == CoAPMessageCode.DELETE ||
                    mCode == CoAPMessageCode.GET ||
                    mCode == CoAPMessageCode.POST ||
                    mCode == CoAPMessageCode.PUT ||
                    mCode == CoAPMessageCode.EMPTY /*special case*/);
        }
        /// <summary>
        /// Check if the given message code represents a response
        /// A response cannot contain GET, POST, PUT, DELETE message codes
        /// </summary>
        /// <param name="mCode">The message code</param>
        /// <returns>boolean</returns>
        public static bool DoesMessageCodeRepresentAResponse(byte mCode)
        {
            return (mCode != CoAPMessageCode.DELETE &&
                    mCode != CoAPMessageCode.GET &&
                    mCode != CoAPMessageCode.POST &&
                    mCode != CoAPMessageCode.PUT);
        }
        /// <summary>
        /// Parse the stream and extract the message code from it
        /// </summary>
        /// <param name="coapMsgStream">The CoAP message stream that contains the message code</param>
        /// <param name="startIndex">The index to start looking for the code</param>
        /// <param name="extraInfo">Not used</param>
        /// <returns>The next index from where next set of information can be extracted from the message stream</returns>
        public int Parse(byte[] coapMsgStream, int startIndex, UInt16 extraInfo)
        {
            if (coapMsgStream == null || coapMsgStream.Length == 0 || startIndex < 0) return startIndex;//do nothing 
            if (coapMsgStream.Length < AbstractCoAPMessage.HEADER_LENGTH) throw new CoAPFormatException("Invalid CoAP message stream");
            if (startIndex >= coapMsgStream.Length) throw new ArgumentException("Start index beyond message stream length");

            this.Value = coapMsgStream[startIndex];
            return (startIndex + 1);
        }
        /// <summary>
        /// Convert this object into a byte stream in network byte order
        /// </summary>
        /// <param name="reserved">Not used now</param>
        /// <returns>byte array</returns>
        public byte[] ToStream(UInt16 reserved)
        {
            byte[] mCode = new byte[1] { this.Value };
            return mCode;
        }
        /// <summary>
        /// Check if the message code is valid or not
        /// </summary>
        /// <param name="value">The value of the message code to check</param>
        /// <returns>bool</returns>
        public bool IsValid(UInt16 value)
        {
            byte valueAsByte = (byte)(value & 0x00FF);
            int classCode = ((valueAsByte & 0xE0) >> 5); //Get uppermost 3-bits
            int detailCode = valueAsByte & 0x1F; //Get last 5 bits
            if (classCode != 0 && classCode != 2 && classCode != 4 && classCode != 5) return false;
            if (classCode == 0 && detailCode > 4) return false;
            if (classCode == 2 && (detailCode == 0 || (detailCode > 5 && detailCode !=31))) return false;
            if (classCode == 4 && (detailCode == 7 || detailCode == 9 || detailCode == 10 ||
                detailCode == 11 || detailCode == 14 || detailCode > 15)) return false;
            if (classCode == 5 && detailCode > 5 ) return false;
            return true;
        }
        #endregion

        #region Overrides
        /// <summary>
        /// String representation of the message code
        /// </summary>
        /// <returns></returns>
        public override string ToString()
        {
            int classCode = ((this.Value & 0xE0) >> 5); //Get uppermost 3-bits
            int detailCode = this.Value & 0x1F; //Get last 5 bits
            string retVal = classCode + "." + ((detailCode < 10) ? "0" + detailCode.ToString() : detailCode.ToString());
            switch (classCode)
            {
                case 0:
                    switch (detailCode)
                    {
                        case 0:
                            return retVal + " Empty";
                        case 1:
                            return retVal + " GET";
                        case 2:
                            return retVal + " POST";
                        case 3:
                            return retVal + " PUT";
                        case 4:
                            return retVal + " DELETE";
                        default:
                            return retVal + " Invalid message code";
                    }
                case 2:
                    switch (detailCode)
                    {
                        case 1:
                            return retVal + " Created";
                        case 2:
                            return retVal + " Deleted";
                        case 3:
                            return retVal + " Valid";
                        case 4:
                            return retVal + " Changed";
                        case 5:
                            return retVal + " Content";
                        case 31:
                            return retVal + " Continue";
                        default:
                            return retVal + " Invalid message code";
                    }
                case 4:
                    switch (detailCode)
                    {
                        case 0:
                            return retVal + " Bad Request";
                        case 1:
                            return retVal + " Unauthorized";
                        case 2:
                            return retVal + " Bad Option";
                        case 3:
                            return retVal + " Forbidden";
                        case 4:
                            return retVal + " Not Found";
                        case 5:
                            return retVal + " Method Not Allowed";
                        case 6:
                            return retVal + " Not Acceptable";
                        case 8:
                            return retVal + " Request entity incomplete";
                        case 12:
                            return retVal + " Precondition Failed";
                        case 13:
                            return retVal + " Request Entity Too Large";
                        case 15:
                            return retVal + " Unsupported Content-Format";
                        default:
                            return retVal + " Invalid message code";
                    }
                case 5:
                    switch (detailCode)
                    {
                        case 0:
                            return retVal + " Internal Server Error";
                        case 1:
                            return retVal + " Not Implemented";
                        case 2:
                            return retVal + " Bad Gateway";
                        case 3:
                            return retVal + " Service Unavailable";
                        case 4:
                            return retVal + " Gateway Timeout";
                        case 5:
                            return retVal + " Proxying Not Supported";
                        default:
                            return retVal + " Invalid message code";
                    }
                default:
                    return retVal + " Invalid message code";
            }
        }
        #endregion
    }
}
