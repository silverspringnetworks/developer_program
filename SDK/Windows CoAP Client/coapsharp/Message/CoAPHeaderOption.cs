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
    /// Represents a single option within the CoAP message
    /// </summary>
    public class CoAPHeaderOption : IParsable
    {
        #region Constants
        /// <summary>
        /// The option If-Match
        /// </summary>
        public const UInt16 IF_MATCH = 1;
        /// <summary>
        /// The option Uri-Host
        /// </summary>
        public const UInt16 URI_HOST = 3;
        /// <summary>
        /// The option ETag
        /// </summary>
        public const UInt16 ETAG = 4;
        /// <summary>
        /// The option If-None-Match
        /// </summary>
        public const UInt16 IF_NONE_MATCH = 5;
        /// <summary>
        /// The option Observe
        /// </summary>
        public const UInt16 OBSERVE = 6;
        /// <summary>
        /// The option Uri-Port
        /// </summary>
        public const UInt16 URI_PORT = 7;
        /// <summary>
        /// The option Location-Path
        /// </summary>
        public const UInt16 LOCATION_PATH = 8;
        /// <summary>
        /// The option Uri-Path
        /// </summary>
        public const UInt16 URI_PATH = 11;
        /// <summary>
        /// The option Content-Format
        /// </summary>
        public const UInt16 CONTENT_FORMAT = 12;
        /// <summary>
        /// The option Max-Age
        /// </summary>
        public const UInt16 MAX_AGE = 14;
        /// <summary>
        /// The option Uri-Query
        /// </summary>
        public const UInt16 URI_QUERY = 15;
        /// <summary>
        /// The option Accept
        /// </summary>
        public const UInt16 ACCEPT = 17;
        /// <summary>
        /// The option Location-Query
        /// </summary>
        public const UInt16 LOCATION_QUERY = 20;
        /// <summary>
        /// The option Block2 for blockwise transfer
        /// </summary>
        public const UInt16 BLOCK2 = 23;
        /// <summary>
        /// The option Block1 for blockwise transfer
        /// </summary>
        public const UInt16 BLOCK1 = 27;
        /// <summary>
        /// The option Size2 for blockwise transfer
        /// </summary>
        public const UInt16 SIZE2 = 28;
        /// <summary>
        /// The option Proxy-Uri
        /// </summary>
        public const UInt16 PROXY_URI = 35;
        /// <summary>
        /// The option Proxy-Scheme
        /// </summary>
        public const UInt16 PROXY_SCHEME = 39;
        /// <summary>
        /// The option Size1 for blockwise transfer
        /// </summary>
        public const UInt16 SIZE1 = 60;        

        #endregion

        #region Implementation
        /// <summary>
        /// Holds the option number
        /// </summary>
        protected UInt16 _optionNumber = 0;
        #endregion

        #region Properties
        /// <summary>
        /// Accessor/Mutator for option number
        /// </summary>
        public UInt16 Number 
        {
            get { return _optionNumber; }
            set
            {
                //if (!this.IsValid(value)) throw new FormatException("Invalid option number");
                this._optionNumber = value;
            }
        }
        /// <summary>
        /// Accessor/Mutator for size of option value in bytes
        /// </summary>
        public UInt16 ValueSizeInBytes { get; set; }
        /// <summary>
        /// The option value
        /// </summary>
        public byte[] Value { get; set; }
        
        #endregion

        #region Constructors
        /// <summary>
        /// Default constructor
        /// </summary>
        public CoAPHeaderOption() { }
        /// <summary>
        /// Overloaded constructor
        /// </summary>
        /// <param name="optionNumber">The option number</param>
        /// <param name="optionValue">The option value</param>
        public CoAPHeaderOption(UInt16 optionNumber, byte[] optionValue)
        {
            int maxLengthOfOptionValue = this.GetOptionValueMaxLengthInBytes(optionNumber);
            int minLengthOfOptionValue = this.GetOptionValueMinLengthInBytes(optionNumber);
            int lengthOfOptionValue = (optionValue == null) ? 0 : optionValue.Length;

            this.Number = optionNumber;
            if (this.IsRecognized())            
                if (lengthOfOptionValue < minLengthOfOptionValue || lengthOfOptionValue > maxLengthOfOptionValue)
                    throw new CoAPFormatException("Invalid length of option value for the given option number " + optionNumber);

            if (this.Number == CoAPHeaderOption.CONTENT_FORMAT)
            {
                CoAPContentFormatOption contentFormat = new CoAPContentFormatOption(AbstractByteUtils.ToUInt16(optionValue));
                if (!contentFormat.IsValid()) throw new CoAPFormatException("Content format not supported");
            }
            
            if (optionValue != null && optionValue.Length > 0)
            {
                this.Value = new byte[optionValue.Length];
                Array.Copy(optionValue , this.Value , optionValue.Length);
                this.ValueSizeInBytes = (UInt16)optionValue.Length;
            }
        }
        #endregion

        #region Parsing Operations
        /// <summary>
        /// Create a header option by parsing the coap data stream
        /// </summary>
        /// <param name="coapStream">The CoAP message stream that contains the option</param>
        /// <param name="startIndex">The index to start looking for the option</param>
        /// <param name="previousOptionNumber">The previous option number in the sequence of options</param>
        /// <returns>The next index from where next set of information can be extracted from the message stream</returns>
        public int Parse(byte[] coapStream, int startIndex, UInt16 previousOptionNumber)
        {
            if (coapStream == null || coapStream.Length < startIndex || startIndex < AbstractCoAPMessage.HEADER_LENGTH)
                throw new ArgumentNullException("CoAP stream is null or start index is invalid");
            int nextIndex = startIndex;
            byte optionDelta = (byte)((coapStream[nextIndex] & 0xF0) >> 4);
            byte optionLength = (byte)(coapStream[nextIndex] & 0x0F);

            UInt16 optionStreamSizeInBytes = 1;

            //Calculate option number
            if (optionDelta < 13)
                this.Number = (UInt16)(optionDelta + previousOptionNumber);
            else if (optionDelta == 13)
            {
                this.Number = (UInt16)((UInt16)coapStream[++nextIndex] + 13 + previousOptionNumber);
                optionStreamSizeInBytes += 1;
            }
            else if (optionDelta == 14)
            {
                byte[] optionDeltaEx = new byte[] { coapStream[++nextIndex], coapStream[++nextIndex] };
                optionDeltaEx = AbstractNetworkUtils.FromNetworkByteOrder(optionDeltaEx);
                this.Number = (UInt16)(AbstractByteUtils.ToUInt16(optionDeltaEx) + (UInt16)269 + previousOptionNumber);
                optionStreamSizeInBytes += 2;
            }
            else if (optionDelta == 15)
                throw new CoAPFormatException("Option delta cannot contain payload marker value");

            //Calculate option value length
            if (optionLength < 13)
                ValueSizeInBytes = optionLength;
            else if (optionLength == 13)
            {
                ValueSizeInBytes = (UInt16)(coapStream[++nextIndex] + 13);
                optionStreamSizeInBytes += 1;
            }
            else if (optionLength == 14)
            {
                byte[] optionLengthEx = new byte[] { coapStream[++nextIndex], coapStream[++nextIndex] };
                optionLengthEx = AbstractNetworkUtils.FromNetworkByteOrder(optionLengthEx);
                ValueSizeInBytes = (UInt16)(AbstractByteUtils.ToUInt16(optionLengthEx) + (UInt16)269);
                optionStreamSizeInBytes += 2;
            }
            else if (optionLength == 15)
                throw new CoAPFormatException("Option length cannot be 0x0F. This is a reserved value");
            //Check if option is of type uint or is empty
            if ((this.NeedsByteOrdering(this.Number) && optionLength == 0) || this.IsEmpty())
            {
                //this means that the value of the option is zero or this is an empty option
                Value = null;
            }
            else if (ValueSizeInBytes > 0) //normal processing
            {
                //Get option Value
                Value = new byte[ValueSizeInBytes];
                Array.Copy(coapStream, ++nextIndex, Value, 0, ValueSizeInBytes);
                if( this.NeedsByteOrdering(this.Number))
                    Value = AbstractNetworkUtils.FromNetworkByteOrder(Value);
                optionStreamSizeInBytes += ValueSizeInBytes;
            }
            return (startIndex + optionStreamSizeInBytes);
        }
        /// <summary>
        /// Convert this option into a byte stream int network byte order
        /// </summary>
        /// <param name="previousOptionNumber">The previous option number in the sequence of options</param>
        /// <returns>byte array</returns>
        public byte[] ToStream(UInt16 previousOptionNumber)
        {
            //The option format is given below

            /*********************************************
            | 4 bits option delta | 4 bits length        |
            ----------------------------------------------
            |      0-2 bytes option delta extended       |
            ----------------------------------------------
            |      0-2 bytes option length extended      |
             ----------------------------------------------
            |               0-* bytes option value       |
             *********************************************/
            int streamLength = 1;
            //Get Option delta
            byte optionDelta = 0;
            UInt16 derivedOptionNumber = (UInt16)(this.Number - previousOptionNumber);

            byte[] optionDeltaEx = null;
            if (derivedOptionNumber <= 12)
            {
                optionDelta = (byte)derivedOptionNumber;
            }
            else if (derivedOptionNumber > 12 && derivedOptionNumber <= 255 /*Option number is single byte*/)
            {
                optionDeltaEx = new byte[1] { (byte)(derivedOptionNumber - 13) };
                optionDelta = 13;                
                streamLength++;//1 additional byte
            }
            else /*Option number is double byte*/
            {
                optionDelta = 14;
                optionDeltaEx = AbstractByteUtils.GetBytes((UInt16)(derivedOptionNumber - 269));
                optionDeltaEx = AbstractNetworkUtils.ToNetworkByteOrder(optionDeltaEx);
                streamLength += 2; //two additional bytes
            }
            //Get option length
            byte optionLength = 0;
            byte[] optionLengthEx = null;
            if (this.Value != null)
            {
                if (this.Value.Length > 0 && this.Value.Length < 13)
                    optionLength = (byte)this.Value.Length;
                else if (this.Value.Length > 12 && this.Value.Length <= 255)
                {
                    optionLength = 13;
                    optionLengthEx = new byte[1] { (byte)(this.Value.Length - 13) };
                    streamLength++;//1 additional byte
                }
                else if (this.Value.Length > 255)
                {
                    optionLength = 14;
                    optionLengthEx = AbstractByteUtils.GetBytes((UInt16)(this.Value.Length - 269));
                    optionLengthEx = AbstractNetworkUtils.ToNetworkByteOrder(optionLengthEx);
                    streamLength += 2; //two additional bytes
                }
            }
            streamLength += ((this.Value != null && !this.IsEmpty()) ? this.Value.Length : 0);
            byte[] optionStream = new byte[streamLength];
            int count = 0;
            optionStream[count++] = (byte)((optionDelta << 4) | (optionLength));//header + length
            if (optionDelta == 13) //delta extended
                optionStream[count++] = optionDeltaEx[0];
            else if (optionDelta == 14)
            {
                optionStream[count++] = optionDeltaEx[0];
                optionStream[count++] = optionDeltaEx[1];
            }
            if (optionLength == 13)
                optionStream[count++] = optionLengthEx[0];
            else if (optionLength == 14)
            {
                optionStream[count++] = optionLengthEx[0];
                optionStream[count++] = optionLengthEx[1];
            }
            if (this.Value != null && this.Value.Length > 0 && !this.IsEmpty())
            {
                byte[] optionValue = new byte[this.Value.Length];
                Array.Copy(this.Value, optionValue, this.Value.Length);
                if (this.NeedsByteOrdering(this.Number))
                    optionValue = AbstractNetworkUtils.ToNetworkByteOrder(optionValue);
                Array.Copy(optionValue, 0, optionStream, count, optionValue.Length);
            }
            
            return optionStream;
        }
        #endregion

        #region Operations
        /// <summary>
        /// Check if this option number is valid or not..basically check if its a recognized option number 
        /// </summary>
        /// <param name="value">The option number</param>
        /// <returns>bool</returns>
        public bool IsValid(UInt16 value)
        {
            if (value != CoAPHeaderOption.ACCEPT && value != CoAPHeaderOption.BLOCK1 &&
                value != CoAPHeaderOption.BLOCK2 && value != CoAPHeaderOption.CONTENT_FORMAT &&
                value != CoAPHeaderOption.ETAG && value != CoAPHeaderOption.IF_MATCH &&
                value != CoAPHeaderOption.IF_NONE_MATCH && value != CoAPHeaderOption.OBSERVE &&
                value != CoAPHeaderOption.LOCATION_PATH && value != CoAPHeaderOption.LOCATION_QUERY && 
                value != CoAPHeaderOption.MAX_AGE && value != CoAPHeaderOption.PROXY_SCHEME && 
                value != CoAPHeaderOption.PROXY_URI && value != CoAPHeaderOption.SIZE1 &&
                value != CoAPHeaderOption.SIZE2 &&
                value != CoAPHeaderOption.URI_HOST && value != CoAPHeaderOption.URI_PATH &&
                value != CoAPHeaderOption.URI_PORT && value != CoAPHeaderOption.URI_QUERY) return false;
            
            return true;
        }
        /// <summary>
        /// Check if this option is a recognized option or not
        /// </summary>
        /// <returns>bool</returns>
        public bool IsRecognized()
        {
            return this.IsValid(Number);
        }
        /// <summary>
        /// Check if this option is critical
        /// </summary>
        /// <returns>bool</returns>
        public bool IsCritical()
        {
            byte lsbOfOption = (byte)(Number & 0xFF);
            return ((lsbOfOption & 0x01) == 0x01);
        }
        /// <summary>
        /// Check if this is an unsafe option
        /// </summary>
        /// <returns>bool</returns>
        public bool IsUnsafe()
        {
            byte lsbOfOption = (byte)(Number & 0xFF);
            return ((lsbOfOption & 0x01) == 0x02);
        }
        /// <summary>
        /// Check if this option is marked as NoCacheKey
        /// </summary>
        /// <returns>bool</returns>
        public bool NoCacheKey()
        {
            byte lsbOfOption = (byte)(Number & 0xFF);
            return ((lsbOfOption & 0x1e) == 0x1c);
        }
        /// <summary>
        /// Check if the option is repeatable or not
        /// </summary>
        /// <returns>bool</returns>
        public bool IsRepeatable()
        {
            if (this.Number == CoAPHeaderOption.IF_MATCH ||
                this.Number == CoAPHeaderOption.ETAG ||
                this.Number == CoAPHeaderOption.LOCATION_PATH ||
                this.Number == CoAPHeaderOption.URI_PATH ||
                this.Number == CoAPHeaderOption.URI_QUERY ||
                this.Number == CoAPHeaderOption.LOCATION_QUERY)
                return true;
            else
                return false;
        }
        /// <summary>
        /// Check if the option is an empty option type...meaning
        /// no value is associated with it
        /// </summary>
        /// <returns>bool</returns>
        public bool IsEmpty()
        {
            return (this.Number == CoAPHeaderOption.IF_NONE_MATCH) ;
        }
        
        /// <summary>
        /// For the given option number, get the maximum length of the option value in bytes
        /// </summary>
        /// <param name="optionNumber">The option number whose length is required</param>
        /// <returns>int</returns>
        public int GetOptionValueMaxLengthInBytes(int optionNumber)
        {
            switch (optionNumber)
            {
                case CoAPHeaderOption.ACCEPT: return 2;
                case CoAPHeaderOption.BLOCK1: return 3;
                case CoAPHeaderOption.BLOCK2: return 3;
                case CoAPHeaderOption.CONTENT_FORMAT: return 2;
                case CoAPHeaderOption.ETAG: return 8;
                case CoAPHeaderOption.IF_MATCH: return 8;
                case CoAPHeaderOption.IF_NONE_MATCH: return 0;
                case CoAPHeaderOption.OBSERVE: return 3;
                case CoAPHeaderOption.LOCATION_PATH: return 255;
                case CoAPHeaderOption.LOCATION_QUERY: return 255;
                case CoAPHeaderOption.MAX_AGE: return 4;
                case CoAPHeaderOption.PROXY_SCHEME: return 255;
                case CoAPHeaderOption.PROXY_URI: return 1034;
                case CoAPHeaderOption.SIZE1: return 4;
                case CoAPHeaderOption.SIZE2: return 4;
                case CoAPHeaderOption.URI_HOST: return 255;
                case CoAPHeaderOption.URI_PATH: return 255;
                case CoAPHeaderOption.URI_PORT: return 2;
                case CoAPHeaderOption.URI_QUERY: return 255; 
                default: return 1024;//TODO::No guidance for options that are not recognized...
            }            
        }
        /// <summary>
        /// For the given option number, get the minimum length of the option value in bytes
        /// </summary>
        /// <param name="optionNumber">The option number whose length is required</param>
        /// <returns>int</returns>
        public int GetOptionValueMinLengthInBytes(int optionNumber)
        {
            switch (optionNumber)
            {
                case CoAPHeaderOption.ACCEPT:
                case CoAPHeaderOption.BLOCK1 :
                case CoAPHeaderOption.BLOCK2 :
                case CoAPHeaderOption.CONTENT_FORMAT :
                case CoAPHeaderOption.IF_MATCH :
                case CoAPHeaderOption.IF_NONE_MATCH :
                case CoAPHeaderOption.OBSERVE :
                case CoAPHeaderOption.LOCATION_PATH :
                case CoAPHeaderOption.LOCATION_QUERY :
                case CoAPHeaderOption.MAX_AGE :
                case CoAPHeaderOption.SIZE1 :
                case CoAPHeaderOption.SIZE2 :
                case CoAPHeaderOption.URI_PATH :
                case CoAPHeaderOption.URI_PORT :
                case CoAPHeaderOption.URI_QUERY:
                    return 0;
                case CoAPHeaderOption.ETAG :
                case CoAPHeaderOption.PROXY_SCHEME :
                case CoAPHeaderOption.PROXY_URI :
                case CoAPHeaderOption.URI_HOST:
                    return 1;
                default:
                    return 0;//TODO::No guidance for options that are not recognized...
            }
        }
        
        #endregion   
     
        #region Overrides
        /// <summary>
        /// Convert to a string representation
        /// </summary>
        /// <returns>string</returns>
        public override string ToString()
        {
            string optionValueAsString = (this.Value != null) ? AbstractByteUtils.ByteToStringUTF8(this.Value) : "";

            switch (this.Number)
            {
                case CoAPHeaderOption.ACCEPT:
                    return "Accept : " + AbstractByteUtils.ToUInt16(this.Value).ToString();
                case CoAPHeaderOption.BLOCK1:
                    CoAPBlockOption cbo1 = new CoAPBlockOption(this.Value);
                    return "Block1 : " + cbo1.ToString();
                case CoAPHeaderOption.BLOCK2:
                    CoAPBlockOption cbo2 = new CoAPBlockOption(this.Value);
                    return "Block2 : " + cbo2.ToString();
                case CoAPHeaderOption.CONTENT_FORMAT:                
                    CoAPContentFormatOption ccformat = new CoAPContentFormatOption(AbstractByteUtils.ToUInt16(this.Value));
                    return "Content-Format : " + ccformat.ToString();                
                case CoAPHeaderOption.ETAG:
                    return "ETag : " + optionValueAsString;
                case CoAPHeaderOption.IF_MATCH:
                    return "If-Match : " + optionValueAsString;
                case CoAPHeaderOption.IF_NONE_MATCH:
                    return "If-None-Match : ";
                case CoAPHeaderOption.OBSERVE:
                    return "Observe : " + AbstractByteUtils.ToUInt64(this.Value).ToString();//We have no data structure for 3-bytes
                case CoAPHeaderOption.LOCATION_PATH:
                    return "Location-Path : " + optionValueAsString;
                case CoAPHeaderOption.LOCATION_QUERY:
                    return "Location-Query : " + optionValueAsString;
                case CoAPHeaderOption.MAX_AGE:
                    return "Max-Age : " + AbstractByteUtils.ToUInt64(this.Value).ToString();
                case CoAPHeaderOption.PROXY_SCHEME:
                    return "Proxy-Scheme : " + optionValueAsString;
                case CoAPHeaderOption.PROXY_URI:
                    return "Proxy-Uri : " + optionValueAsString;
                case CoAPHeaderOption.SIZE1:
                    return "Size1 : " + AbstractByteUtils.ToUInt64(this.Value).ToString();
                case CoAPHeaderOption.SIZE2:
                    return "Size2 : " + AbstractByteUtils.ToUInt64(this.Value).ToString();
                case CoAPHeaderOption.URI_HOST:
                    return "Uri-Host : " + optionValueAsString;
                case CoAPHeaderOption.URI_PATH:
                    return "Uri-Path : " + optionValueAsString;
                case CoAPHeaderOption.URI_PORT:
                    return "Uri-Port : " + AbstractByteUtils.ToUInt16(this.Value);
                case CoAPHeaderOption.URI_QUERY:
                    return "Uri-Query : " + optionValueAsString;
                default:
                    return "Unknown : " + optionValueAsString;
            }                        
        }
        #endregion

        #region Helpers
        /// <summary>
        /// Options are of type opaque,empty,string, uint.
        /// Only uint types require byte order management.
        /// </summary>
        /// <param name="optionNumber">The option number</param>
        /// <returns>bool</returns>
        protected bool NeedsByteOrdering(UInt16 optionNumber)
        {
            return (optionNumber == CoAPHeaderOption.URI_PORT ||
                optionNumber == CoAPHeaderOption.OBSERVE ||
                optionNumber == CoAPHeaderOption.CONTENT_FORMAT ||
                optionNumber == CoAPHeaderOption.MAX_AGE ||
                optionNumber == CoAPHeaderOption.ACCEPT ||
                optionNumber == CoAPHeaderOption.SIZE1 || 
                optionNumber == CoAPHeaderOption.SIZE2 || 
                optionNumber == CoAPHeaderOption.BLOCK1 || 
                optionNumber == CoAPHeaderOption.BLOCK2) ;
        }
        #endregion
    }
}
