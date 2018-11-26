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

using System.Collections;

namespace EXILANT.Labs.CoAP.Helpers
{
    /// <summary>
    /// A result object of type Text. This can be used to exchange Text specific payload
    /// </summary>
    public class TextResult
    {
        /// <summary>
        /// Convert a Hashtable entries into Text. The format is key=value;key=value
        /// Only number, string, boolean and datetime values are supported.
        /// Datetime is converted to ddMMyyyy HHmmss format in string
        /// If you want to pass an array, pass a comma separated list of values (as string).
        /// Semi-Colon (;) in data are converted to ~ and comma(,) in data are converted to `
        /// </summary>
        /// <param name="ht">Hashtable with Key/Value pairs</param>
        /// <returns>string</returns>
        public static string ToText(Hashtable ht)
        {
            if (ht == null || ht.Count == 0) return "";
            StringBuilder sb = new StringBuilder();
            int count = 0;
            
            foreach (string key in ht.Keys)
            {
                if (count > 0) sb.Append(";");
                Object value = ht[key];
                string modKey = AbstractStringUtils.Replace(key, ';', '~');
                modKey = AbstractStringUtils.Replace(modKey, ',', '`');
                if (value != null)
                    sb.Append("\"").Append(modKey).Append("\"=").Append(TextResult.AsString(value));
                else
                    sb.Append("\"").Append(modKey).Append("\"= \"\"");
                count++;
            }
            return sb.ToString();
        }
        /// <summary>
        /// Convert to a Hashtable with key/value pairs from a Text string.
        /// ~ character in key/value is changed to ;
        /// ` character in key/value is changed to ,
        /// </summary>
        /// <param name="str">The string</param>
        /// <returns>Hashtable</returns>
        public static Hashtable FromText(string str)
        {
            Hashtable result = new Hashtable();
            if (str == null || str.Trim().Length == 0) throw new ArgumentNullException("Cannot convert a NULL/empty string");
            str = str.Trim();
            
            //json = AbstractStringUtils.Replace(json, '\r', ' ');
            //json = AbstractStringUtils.Replace(json, '\n', ' ');

            string[] keyVals = str.Split(new char[] { ';' });
            foreach (string keyValPair in keyVals)
            {
                if (keyValPair.Trim().Length == 0) continue;
                string[] parts = keyValPair.Split(new char[] { '=' });
                parts[0] = AbstractStringUtils.Replace(parts[0], '\"', ' ').Trim();
                parts[1] = AbstractStringUtils.Replace(parts[1], '\"', ' ').Trim();
                //character replacements
                parts[0] = AbstractStringUtils.Replace(parts[0], '~', ';');
                parts[0] = AbstractStringUtils.Replace(parts[0], '`', ',');
                parts[1] = AbstractStringUtils.Replace(parts[1], '~', ';');
                parts[1] = AbstractStringUtils.Replace(parts[1], '`', ',');
                result.Add(parts[0], parts[1]);
            }

            return result;
        }
        /// <summary>
        /// Convert an object into string representation.
        /// Dictionary, Enumerable, DictionaryEntry and Class are
        /// simply converted to string...(not supported essentially)
        /// DateTime is always returned as ddMMyyyy HHmmss
        /// Semi-Colon (;) in data are converted to ~ and comma(,) in data are converted to `
        /// </summary>
        /// <param name="obj">Object instance to convert to JSON</param>
        /// <returns>string</returns>
        protected static string AsString(Object obj)
        {
            if (obj == null) return "";
            Type type = obj.GetType();
            string objAsStr = obj.ToString();

            objAsStr = AbstractStringUtils.Replace(objAsStr, ';', '~');
            objAsStr = AbstractStringUtils.Replace(objAsStr, ',', '`');

            switch (type.Name)
            {
                case "Boolean":
                    return ((bool)obj) ? "true" : "false";
                case "String":
                case "Char":
                case "Guid":
                    return "\"" + objAsStr + "\"";
                case "Single":
                case "Double":
                case "Decimal":
                case "Float":
                case "Byte":
                case "SByte":
                case "Int16":
                case "UInt16":
                case "Int32":
                case "UInt32":
                case "Int64":
                case "UInt64":
                    return objAsStr;
                case "DateTime":
                    DateTime dt = (DateTime)obj;
                    return "\"" + AbstractStringUtils.PadLeft(dt.Day.ToString() , '0', 2) +
                                AbstractStringUtils.PadLeft(dt.Month.ToString(), '0', 2) +
                                AbstractStringUtils.PadLeft(dt.Year.ToString() , '0', 4) + " " +
                                AbstractStringUtils.PadLeft(dt.Hour.ToString(), '0', 2) +
                                AbstractStringUtils.PadLeft(dt.Minute.ToString(), '0', 2) +
                                AbstractStringUtils.PadLeft(dt.Second.ToString(), '0', 2) + "\"";
                default:
                    return "\"" + objAsStr + "\"";
            }
        }
    }
}
