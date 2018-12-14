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
using EXILANT.Labs.CoAP.Helpers;
using EXILANT.Labs.CoAP.Exceptions;
using System.Net;
using System.Collections;

namespace EXILANT.Labs.CoAP.Message
{
    /// <summary>
    /// This abstract class simply represents a CoAP message. A CoAP message in itself has no meaning, unless
    /// its tied to an operation type (e.g. request, response). Thus, this is made an abstract class
    /// </summary>
    public abstract class AbstractCoAPMessage
    {
        #region Constants
        /// <summary>
        /// Holds the constant value of header length for a CoAP message
        /// </summary>
        public const byte HEADER_LENGTH = 4;        

        #endregion

        #region Properties
        /// <summary>
        /// Accessor/Mutator for the version
        /// </summary>
        public CoAPVersion Version { get; set; }
        /// <summary>
        /// Accessor/Mutator for the message type
        /// </summary>
        public CoAPMessageType MessageType { get; set; }
        /// <summary>
        /// Accessor/Mutator for the token associated with the message
        /// </summary>
        public CoAPToken Token { get; set; }
        /// <summary>
        /// Accessor/Mutator for the message code associated with this message
        /// </summary>
        public CoAPMessageCode Code { get; set; }
        /// <summary>
        /// Accessor/Mutator for the message ID
        /// </summary>
        public CoAPMessageID ID { get; set; }
        /// <summary>
        /// Accessor/Mutator for the message options
        /// </summary>
        public CoAPHeaderOptions Options { get; set; }
        /// <summary>
        /// Accessor/Mutator for the payload associated with this message
        /// </summary>
        public CoAPPayload Payload { get; set; }
        /// <summary>
        /// Accessor/Mutator for timeout value in seconds for this message
        /// </summary>
        public int Timeout { get; set; }
        /// <summary>
        /// Accessor/Mutator to manage the re-transmission count of this message
        /// </summary>
        public int RetransmissionCount { get; set; }
        /// <summary>
        /// Accessor/Mutator for the remote endpoint that sent this message
        /// </summary>
        public EndPoint RemoteSender { get; set; }
        /// <summary>
        /// Accessor/Mutator to check if this is a secure message
        /// </summary>
        public bool IsSecure { get; set; }
        /// <summary>
        /// When was this message dispatched...we do not need exact timings
        /// </summary>
        public DateTime DispatchDateTime { get; set; }
        #endregion
        
        #region Format Converters
        /// <summary>
        /// Populate this message object from a message stream
        /// </summary>
        /// <param name="coapMsgStream">The CoAP message stream</param>
        public virtual void FromByteStream(byte[] coapMsgStream)
        {
            if (coapMsgStream == null || coapMsgStream.Length < AbstractCoAPMessage.HEADER_LENGTH /*minimum header length*/)
                throw new CoAPFormatException("Cannot parse.Invalid stream length.");

            int startIndex = 0;
            //first two bits provide version number
            this.Version = new CoAPVersion();
            startIndex = this.Version.Parse(coapMsgStream, startIndex, 0);
            //Next two bits provide the message type
            this.MessageType = new CoAPMessageType();
            startIndex = this.MessageType.Parse(coapMsgStream, startIndex, 0);
            //Next 4 bits provide the token value...if token value is > 0 then token follows the header                        
            this.Token = new CoAPToken();
            startIndex = this.Token.Parse(coapMsgStream, startIndex, 0);
            //Next 8 bits contain the message code
            this.Code = new CoAPMessageCode();
            startIndex = this.Code.Parse(coapMsgStream, startIndex, 0);
            //next two bytes (16-bits) provide the message Id
            this.ID = new CoAPMessageID();
            startIndex = this.ID.Parse(coapMsgStream, startIndex, 0);
            //Get the header options...they will start after the token value
            startIndex += this.Token.Length;
            //Parse the options
            this.Options = new CoAPHeaderOptions();
            startIndex = this.Options.Parse(coapMsgStream, startIndex, 0); //Parse the stream, find out the options, return the next location
            //Get the payload
            this.Payload = new CoAPPayload();
            this.Payload.Parse(coapMsgStream, startIndex, 0);
        }
        /// <summary>
        /// Convert this CoAP message to a byte stream in network byte order
        /// </summary>
        /// <returns>byte array</returns>
        public byte[] ToByteStream()
        {
            int streamLength = AbstractCoAPMessage.HEADER_LENGTH + /*mandatory header*/
                                ((this.Token != null) ? this.Token.Length : 0) + /*Token holder*/
                                ((this.Options != null) ? this.Options.GetLengthOfOptionsStream() : 0)+ /*Options holder*/
                               ((this.Payload !=null && this.Payload.Value != null) ? this.Payload.Value.Length + 1 : 0); /*Payload including marker*/
            
            byte[] coapStream = new byte[streamLength];
            int count=0;
            byte[] chunkStream = null;
            /*version+type+token length*/

            if (this.Version == null) this.Version = new CoAPVersion(); //default version
            coapStream[count++] = (byte)(this.Version.ToStream(0)[0] | this.MessageType.ToStream(0)[0] | ((this.Token != null) ? this.Token.ToStream(0)[0] : 0));
            /*code*/
            coapStream[count++] = this.Code.ToStream(0)[0];
            /*message id*/
            chunkStream = this.ID.ToStream(0);
            Array.Copy(chunkStream, 0, coapStream, count, chunkStream.Length);
            count += chunkStream.Length;
            /*token*/
            if (this.Token != null && this.Token.Length > 0)
            {
                chunkStream = this.Token.ToStream(0);//first element contains the length
                Array.Copy(chunkStream, 1, coapStream, count, chunkStream.Length - 1);
                count += chunkStream.Length - 1;
            }
            /*options*/
            if (this.Options != null && this.Options.Count > 0)
            {
                int lengthOfOptions = this.Options.GetLengthOfOptionsStream();
                if (lengthOfOptions > 0)
                {
                    chunkStream = this.Options.ToStream(0);
                    Array.Copy(chunkStream, 0, coapStream, count, lengthOfOptions);
                    count += lengthOfOptions;
                }
            }
            /*payload marker + payload*/
            if (this.Payload != null && this.Payload.Value != null && this.Payload.Value.Length > 0)
            {
                coapStream[count++] = 0xFF;
                chunkStream = this.Payload.ToStream(0);
                Array.Copy(chunkStream, 0, coapStream, count, this.Payload.Value.Length);
            }
            return coapStream;
        }
        #endregion

        #region Message Parts Management
        /// <summary>
        /// Add the token value. If the value is larger than 8 bytes, then it is truncated to 8 bytes
        /// The string is treated as UTF-8 string
        /// </summary>
        /// <param name="tokenValue">The token value (maximum 8 bytes)</param>
        public void AddTokenValue(string tokenValue)
        {
            if (this.Token == null) this.Token = new CoAPToken();
            if (tokenValue != null)
            {
                byte[] asBytes = AbstractByteUtils.StringToByteUTF8(tokenValue);
                int len = (asBytes.Length > 8) ? 8 : asBytes.Length;
                this.Token.Value = new byte[len];
                Array.Copy(asBytes, 0, this.Token.Value, 0, len);
                this.Token.Length = (byte)this.Token.Value.Length;
            }            
        }
        /// <summary>
        /// Get the token value
        /// </summary>
        /// <returns>long</returns>
        public UInt64 GetTokenValue()
        {
            if (this.Token == null || this.Token.Length == 0)
                return 0;
            else
                return AbstractByteUtils.ToUInt64(this.Token.Value);
        }
        /// <summary>
        /// Add the message Id to this message
        /// </summary>
        /// <param name="mId">The message Id</param>
        public void AddMessageId(UInt16 mId)
        {
            if (this.ID == null) this.ID = new CoAPMessageID();
            this.ID.Value = mId;
        }
        /// <summary>
        /// Get the message id
        /// </summary>
        /// <returns>The message id</returns>
        public UInt16 GetMessageId()
        {
            if (this.ID == null)
                return 0;
            else
                return this.ID.Value;
        }
        /// <summary>
        /// Add an option to this request
        /// </summary>
        /// <param name="optionNumber">The option number</param>
        /// <param name="optionValue">The option value</param>
        public void AddOption(UInt16 optionNumber , byte[] optionValue)
        {
            if (this.Options == null) this.Options = new CoAPHeaderOptions();
            this.Options.AddOption(optionNumber, optionValue);
        }
        /// <summary>
        /// Add the payload...limited to 1024 bytes...we assume the string is in UTF-8 format
        /// </summary>
        /// <param name="payload">A unicode string not longer than 1024 bytes</param>
        public void AddPayload(string payload)
        {
            if(payload == null || payload.Trim().Length == 0) return;
            
            this.Payload = new CoAPPayload();
            byte[] payloadAsUtf8 = AbstractByteUtils.StringToByteUTF8(payload);
            int length = payloadAsUtf8.Length;
            //if (length > 1024) length = 1024;

            this.Payload.Value = new byte[length];
            Array.Copy(payloadAsUtf8, 0, this.Payload.Value, 0, length);
        }
        /// <summary>
        /// Get the payload as a string (A UTF-8 formatted string)
        /// </summary>        
        /// <returns>The payload as string</returns>
        public string GetPayload()
        {
            if (this.Payload == null || this.Payload.Value == null || this.Payload.Value.Length == 0) return null;

            return AbstractByteUtils.ByteToStringUTF8(this.Payload.Value);
        }
        /// <summary>
        /// Set the content format of the payload
        /// </summary>
        /// <param name="contentFormat">The content format identifier</param>
        public void SetContentFormat(CoAPContentFormatOption contentFormat)
        {
            if (this.Options == null) this.Options = new CoAPHeaderOptions();
            this.Options.Add(new CoAPHeaderOption(CoAPHeaderOption.CONTENT_FORMAT, contentFormat.GetValueAsBytes()));
        }
        #endregion

        #region Message Type Identifiers
        /// <summary>
        /// From the given byte stream, just check what is the type of the message (CON/NON/ACK/RST)
        /// </summary>
        /// <param name="data">The data stream to parse</param>
        /// <returns>The message type identifier</returns>
        public static byte PeekMessageType(byte[] data)
        {
            if (data == null || data.Length == 0) throw new ArgumentNullException("Invalid byte stream to parse");            
            byte mType = (byte)((data[0] & 0x30) >> 4);
            return mType;
        }
        /// <summary>
        /// Try to check if the message code represents a request code
        /// </summary>
        /// <param name="data">The data stream to parse</param>
        /// <returns>true if message code found and represents a request</returns>
        public static bool PeekIfMessageCodeIsRequestCode(byte[] data)
        {
            if (data == null || data.Length < AbstractCoAPMessage.HEADER_LENGTH) throw new ArgumentNullException("Invalid byte stream to parse");
            byte mCode = data[1];
            CoAPMessageCode msgCode = new CoAPMessageCode();
            return (msgCode.IsValid(data[1]) && CoAPMessageCode.DoesMessageCodeRepresentARequest(data[1]));
        }
        /// <summary>
        /// From the given byte stream, just check what is the message Id (CON/NON/ACK/RST).
        /// The message byte stream might be corrupted, so we only make a best case attempt
        /// </summary>
        /// <param name="data">The data stream to parse</param>
        /// <returns>The message type identifier</returns>
        public static UInt16 PeekMessageID(byte[] data)
        {
            if (data == null || data.Length < AbstractCoAPMessage.HEADER_LENGTH) throw new ArgumentNullException("Invalid byte stream to parse");
            UInt16 mId = 0;
            try
            {
                byte[] mIdBytes = new byte[2];
                Array.Copy(data, 2, mIdBytes, 0, 2);
                mId = AbstractByteUtils.ToUInt16(mIdBytes);
            }
            catch { }
            return mId;
        }
        /// <summary>
        /// If this is a request, then check if the request is for observation registration.
        /// If response, then check if the response is for an observered value
        /// </summary>
        /// <returns>bool</returns>
        public bool IsObservable()
        {
            return (this.Options != null && this.Options.HasOption(CoAPHeaderOption.OBSERVE)) ;
        }
        /// <summary>
        /// Set the block option
        /// </summary>
        /// <param name="blockNumber">Which block number you are requesting (1 or 2)</param>
        /// <param name="blockOption">The block option</param>
        public void SetBlockOption(UInt16 blockNumber, CoAPBlockOption blockOption)
        {
            if (blockNumber != CoAPHeaderOption.BLOCK1 && blockNumber != CoAPHeaderOption.BLOCK2)
                throw new ArgumentException("Invalid block number. Must be either Block1 or Block2");
            if (blockOption == null) throw new ArgumentNullException("Block option cannot be NULL");

            if (this.Options != null && this.Options.HasOption(blockNumber))
                this.Options.RemoveOption(blockNumber);
            this.AddOption(blockNumber, blockOption.ToStream(0));
        }
        /// <summary>
        /// Get the block 1 option
        /// </summary>
        /// <param name="blockNumber">Which block number you are requesting (1 or 2)</param>
        /// <returns>The block option</returns>
        public CoAPBlockOption GetBlockOption(UInt16 blockNumber)
        {
            if (blockNumber != CoAPHeaderOption.BLOCK1 && blockNumber != CoAPHeaderOption.BLOCK2)
                throw new ArgumentException("Invalid block number. Must be either Block1 or Block2");
            CoAPBlockOption blockOption = null;
            if (this.Options != null && this.Options.HasOption(blockNumber))
            {
                blockOption = new CoAPBlockOption(this.Options.GetOption(blockNumber).Value);
            }
            return blockOption;
        }
        
        #endregion

        #region Validators
        /// <summary>
        /// Check if the message is valid or not
        /// </summary>
        /// <returns>bool</returns>
        public virtual bool IsValid()
        {
            return ( (this.Version != null && this.Version.IsValid( this.Version.CurrentVersion ) ) &&
                    (this.MessageType != null && this.MessageType.IsValid( this.MessageType.Value )) &&
                    (this.Token == null || (this.Token.IsValid( this.Token.Length ))) &&
                    (this.Code != null && this.Code.IsValid( this.Code.Value )) &&
                    (this.ID != null && this.ID.IsValid( this.ID.Value )) &&
                    (this.Options == null || this.Options.IsValid(0)) &&
                    (this.Payload == null || this.Payload.IsValid(0))
                );
        }
        #endregion        

        #region Overrides
        /// <summary>
        /// Convert this message to a human readable string for debugging
        /// </summary>
        /// <returns>String</returns>
        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine(this.Version.ToString()).Append(" ")
                .AppendLine(this.MessageType.ToString()).Append(" ");
            if (this.Token != null)
                sb.AppendLine(this.Token.ToString()).Append(" ");
            else
                sb.AppendLine("Token = NULL");
            sb.AppendLine("Code = " + this.Code.ToString()).Append(" ");
            sb.AppendLine(this.ID.ToString()).Append(" ");
            sb.AppendLine(this.Options.ToString()).Append(" ");
            if (this.Payload != null) sb.AppendLine("Payload=" + this.Payload.ToString()).Append(" ");
            return sb.ToString();
        }
        
        #endregion
    }
}
