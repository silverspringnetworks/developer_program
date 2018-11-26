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

namespace EXILANT.Labs.CoAP.Helpers
{
    /// <summary>
    /// A utilities class for numbers
    /// </summary>
    public abstract class AbstractNumberUtils
    {
        /// <summary>
        /// Convert a string representing a hexadecimal number to UInt32
        /// </summary>
        /// <param name="hexStr">The hex string</param>
        /// <returns>The unsigned integer representation of the hex string</returns>
        public static UInt32 Hex2UInt32(string hexStr)
        {
            if (hexStr == null) throw new ArgumentNullException("Hex string is null");
            if (hexStr.Trim().Length == 0) throw new ArgumentException("Hex string is empty");

            hexStr = (hexStr.Trim().IndexOf("0x") == 0) ? hexStr.Trim().Substring(2).ToUpper() : hexStr.Trim().ToUpper();
            if (hexStr.Length > 8) //more than 4 bytes or 8-nibbles
                throw new ArgumentException("Hex string too large for conversion");

            ArrayList allowedHexChars = new ArrayList();
            char[] hexCharArr = "0123456789ABCDEF".ToCharArray();
            foreach (char hexChar in hexCharArr)
                allowedHexChars.Add(hexChar);

            //check if the hex string contains dis-allowed characters
            char[] hexChars = AbstractStringUtils.PadLeft(hexStr, '0', 8).ToCharArray();
            foreach (char hex in hexChars)
                if (!allowedHexChars.Contains(hex)) throw new ArgumentException("Input string does not represent hexadecimal characters");

            UInt32 mul = 1;
            UInt32 result = 0;
            for (int count = hexChars.Length - 1; count >= 0; --count)
            {
                result += (UInt32)(mul * (allowedHexChars.IndexOf(hexChars[count])));
                mul = (uint)(mul * allowedHexChars.Count);
            }

            return result;
        }
        /// <summary>
        /// Convert the UInt32 value to its hex representation
        /// </summary>
        /// <param name="input">The input to convert to hex representation</param>
        /// <param name="minLen">
        /// The minimum length of the output string. 
        /// </param>
        /// <returns>string</returns>
        public static string UInt32ToHex(UInt32 input, int minLen)
        {
            return input.ToString("X" + minLen.ToString());
        }
    }
}
