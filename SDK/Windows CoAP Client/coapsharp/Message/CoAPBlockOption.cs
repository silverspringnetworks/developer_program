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

namespace EXILANT.Labs.CoAP.Message
{
    /// <summary>
    /// CoAP protocol extension supports Block transfers.
    /// This class holds the information related to the block that is 
    /// being carried in the payload (descriptive or control usage)
    /// </summary>
    public class CoAPBlockOption
    {
        #region Constants
        /// <summary>
        /// Indicates a block size of 16 bytes
        /// </summary>
        public const byte BLOCK_SIZE_16 = 0;
        /// <summary>
        /// Indicates a block size of 32 bytes
        /// </summary>
        public const byte BLOCK_SIZE_32 = 1;
        /// <summary>
        /// Indicates a block size of 64 bytes
        /// </summary>
        public const byte BLOCK_SIZE_64 = 2;
        /// <summary>
        /// Indicates a block size of 128 bytes
        /// </summary>
        public const byte BLOCK_SIZE_128 = 3;
        /// <summary>
        /// Indicates a block size of 256 bytes
        /// </summary>
        public const byte BLOCK_SIZE_256 = 4;
        /// <summary>
        /// Indicates a block size of 512 bytes
        /// </summary>
        public const byte BLOCK_SIZE_512 = 5;
        /// <summary>
        /// Indicates a block size of 1024 bytes
        /// </summary>
        public const byte BLOCK_SIZE_1024 = 6;
        #endregion

        #region Implementation
        /// <summary>
        /// Holds the block sequence number
        /// </summary>
        protected UInt32 _seqNumber = 0;
        /// <summary>
        /// Indicates if there are more blocks following this block
        /// </summary>
        protected bool _hasMore = false;
        /// <summary>
        /// The size exponent
        /// </summary>
        protected byte _sizeExp = 0;
        #endregion

        #region Properties
        /// <summary>
        /// Accessor/Mutator for the sequence number of the block
        /// </summary>
        public UInt32 SequenceNumber
        {
            get { return this._seqNumber; }
            set { this._seqNumber = value; }
        }
        /// <summary>
        /// Accessor/Mutator to check if more blocks are following this block
        /// </summary>
        public bool HasMoreBlocks
        {
            get { return this._hasMore; }
            set { this._hasMore = value; }
        }
        /// <summary>
        /// Accessor/Mutator for the size exponent
        /// </summary>
        public byte SizeExponent 
        {
            get { return this._sizeExp; }
            set {
                if (value < CoAPBlockOption.BLOCK_SIZE_16 || value > CoAPBlockOption.BLOCK_SIZE_1024)
                    throw new ArgumentException("Invalid block size exponent");
                this._sizeExp = value;
            }
        }
        /// <summary>
        /// Accessor for the maximum block size
        /// </summary>
        public UInt16 MaxBlockSize
        {
            get { return (UInt16)(System.Math.Pow(2, this.SizeExponent + 4)); }
        }
        #endregion

        #region Constructors
        /// <summary>
        /// Default constructor
        /// </summary>
        public CoAPBlockOption() { }
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="seqNumber">The block sequence number</param>
        /// <param name="hasMore">If true, indicates there are more bytes following this block</param>
        /// <param name="sizeExp">The size exponent</param>
        public CoAPBlockOption(UInt32 seqNumber, bool hasMore, byte sizeExp)
        {
            if (sizeExp < CoAPBlockOption.BLOCK_SIZE_16 || sizeExp > CoAPBlockOption.BLOCK_SIZE_1024)
                throw new ArgumentException("Invalid block size exponent");
            this._seqNumber = seqNumber;
            this._hasMore = hasMore;
            this._sizeExp = sizeExp;
        }
        /// <summary>
        /// The byte array from which we need to construct this block option.
        /// </summary>
        /// <param name="data">
        /// The byte array (max 3 bytes).Endian-ness is taken care of in this method
        /// </param>
        public CoAPBlockOption(byte[] data)
        {
            if (data == null || data.Length == 0 || data.Length > 3)
                throw new ArgumentNullException("Block option byte array must be 1-3 bytes in length"); 

            if (data.Length == 1 /*single byte*/)
            {
                this._seqNumber = (UInt16)(data[0] >> 4);
                this._hasMore = (((data[0] & 0x08) >> 3 ) == 1 ) ? true : false;
                this._sizeExp = (byte)(data[0] & 0x07);
            }
            else if (data.Length == 2 /*double byte*/)
            {
                if (AbstractByteUtils.IsTargetLittleEndian())
                {
                    this._seqNumber = (UInt16)(AbstractByteUtils.ToUInt16(data) >> 4);
                    this._hasMore = ( ((data[0] & 0x08) >> 3)  == 1) ? true : false;
                    this._sizeExp = (byte)(data[0] & 0x07); 
                }
                else
                {
                    this._seqNumber = (UInt16)(AbstractByteUtils.ToUInt16(data) << 4);
                    this._hasMore = (((data[1] & 0x08) >> 3) == 1) ? true : false;
                    this._sizeExp = (byte)(data[1] & 0x07); 
                }                
            }
            else if (data.Length == 3 /*Triple byte*/)
            {
                if (AbstractByteUtils.IsTargetLittleEndian())
                {
                    this._seqNumber = (UInt32) (AbstractByteUtils.ToUInt64(data) >> 4);
                    this._hasMore = (((data[0] & 0x08) >> 3) == 1) ? true : false;
                    this._sizeExp = (byte)(data[0] & 0x07);
                }
                else
                {
                    this._seqNumber = (UInt32)(AbstractByteUtils.ToUInt64(data) << 4);
                    this._hasMore = (((data[2] & 0x08) >> 3) == 1) ? true : false;
                    this._sizeExp = (byte)(data[2] & 0x07);
                } 
            }
        }
        #endregion

        #region Operations
        /// <summary>
        /// As bytes...endian ness is taken care of in this method
        /// </summary>
        /// <param name="reserved">Not used now</param>
        /// <returns>byte[]</returns>
        public byte[] ToStream(UInt16 reserved)
        {            
            if (this.SequenceNumber <= 0xF)
            {
                //single byte
                byte[] retVal = new byte[1];
                retVal[0] = (byte)this.SequenceNumber;
                retVal[0] = (byte)(retVal[0] << 4);//seq number in top 4 bits
                retVal[0] = (this.HasMoreBlocks) ? (byte)(retVal[0] | 0x08) : retVal[0]; //more flag in 5th bit
                retVal[0] = (byte)(retVal[0] | this.SizeExponent);//size exponent, last 3 bits

                return retVal;
            }
            else if (this.SequenceNumber > 0xF && this.SequenceNumber <= 0xFFF)
            {
                //double byte       
                byte[] temp = new byte[2];
                
                if (AbstractByteUtils.IsTargetLittleEndian())
                {
                    temp = AbstractByteUtils.GetBytes((UInt16)(this._seqNumber << 4));
                    temp[0] = (this.HasMoreBlocks) ? (byte)(temp[0] | 0x08) : (byte)(temp[0] & 0xF7);
                    temp[0] = (byte)(temp[0] | this.SizeExponent);
                }
                else
                {
                    temp = AbstractByteUtils.GetBytes((UInt16)(this._seqNumber >> 4));
                    temp[1] = (this.HasMoreBlocks) ? (byte)(temp[1] | 0x08) : (byte)(temp[1] & 0xF7);
                    temp[1] = (byte)(temp[1] | this.SizeExponent);
                }
                return temp;
            }
            else
            {
                //3 bytes...we start with 4(long) and then return last 3 only
                byte[] temp = new byte[4];
                byte[] retVal = new byte[3];
                
                if (AbstractByteUtils.IsTargetLittleEndian())
                {
                    temp = AbstractByteUtils.GetBytes((UInt64)(this._seqNumber << 4));
                    temp[0] = (this.HasMoreBlocks) ? (byte)(temp[0] | 0x08) : (byte)(temp[0] & 0xF7);
                    temp[0] = (byte)(temp[0] | this.SizeExponent);
                    Array.Copy(temp, 0, retVal, 0, 3);
                }
                else
                {
                    //use the index  =2, index 3 will be discarded
                    temp = AbstractByteUtils.GetBytes((UInt64)(this._seqNumber >> 4));
                    temp[2] = (this.HasMoreBlocks) ? (byte)(temp[1] | 0x08) : (byte)(temp[1] & 0xF7);
                    temp[2] = (byte)(temp[1] | this.SizeExponent);
                    Array.Copy(temp, 1, retVal, 0, 3);
                }

                return retVal;
            }
        }
        #endregion

        #region Overrides
        /// <summary>
        /// To string
        /// </summary>
        /// <returns>string</returns>
        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            sb.Append( "SEQ NO=" ).Append(this._seqNumber.ToString()).Append(",")
                .Append("MORE=").Append(this._hasMore.ToString()).Append(",") 
                .Append("SIZE EXP=").Append(this._sizeExp.ToString());

            return sb.ToString();
        }
        #endregion
    }
}
