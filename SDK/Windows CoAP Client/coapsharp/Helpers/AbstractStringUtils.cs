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


namespace EXILANT.Labs.CoAP.Helpers
{
    /// <summary>
    /// A utility class for string object
    /// </summary>
    public abstract class AbstractStringUtils
    {
        /// <summary>
        /// Replace a given character with another within a string
        /// </summary>
        /// <param name="input">The input string where characters have to be replaced</param>
        /// <param name="thisOne">Which character to replace</param>
        /// <param name="withThis">Which character to replace with</param>
        /// <returns>string</returns>
        public static string Replace(string input, char thisOne, char withThis)
        {
            if (input == null) return null;
            if (thisOne == withThis) return input;
            char[] chars = input.ToCharArray();
            for (int count = 0; count < chars.Length; count++)
                if (chars[count] == thisOne) chars[count] = withThis;
            return new string(chars);
        }
        /// <summary>
        /// Pad the left side of the string with the given character
        /// </summary>
        /// <param name="input">The input string to pad with</param>
        /// <param name="padWith">Which character to use to do padding</param>
        /// <param name="length">The length of the resulting string</param>
        /// <returns>string</returns>
        public static string PadLeft(string input, char padWith, int length)
        {
            if (input == null) return null;
            if (input.Length >= length) return input;
            string paddedStr = input;
            while (paddedStr.Length < length)
                paddedStr = new string(padWith, 1) + paddedStr;
            return paddedStr;
        }
    }
}
