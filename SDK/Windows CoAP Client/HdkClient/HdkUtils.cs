/*
Silver Spring Networks, Inc. ("Silver Spring") licenses this software to you and the company (“You” or “Your”) 
a license for this software (“Software”).  
Using the Software constitutes Your acceptance of these terms (“License”).  

License Grant. Subject to Your acceptance and compliance with this License, Silver Spring grants to You, solely for Your own internal business purpose, 
               a non-exclusive, non-transferable license to access and use the Software and the associated user documentation (“Documentation”) 
               for the term and number agreed to be Silver Spring. 

Restrictions. No intellectual property rights are granted under this License and Silver Spring reserves all rights not expressly granted. 
You may not:  
(a) modify or create derivative works of the Software or Documentation; 
(b) assign, transfer, lease or sublicense the Software or Documentation to any third party 
    (other than Your consultants who are bound to written obligations of confidentiality at least as restrictive as those contained in this License); 
and (c) reverse engineer, disassemble, decrypt, extract or otherwise reduce the Software to a human perceivable 
    form or otherwise attempt to determine the source code or algorithms of the Software 
    (unless the foregoing restriction is expressly prohibited by applicable law).
You may not remove or destroy any proprietary, trademark or copyright markings or notices placed on or contained in the Software or Documentation.  
Silver Spring PROVIDES THE SOFTWARE “AS IS” AND MAKES NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY REGARDING OR RELATING TO THE SOFTWARE.  
Silver Spring HEREBY DISCLAIMS ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, 
WHETHER RIGHTS ARISES BY LAW, BY USE AND CUSTOM OR USAGE IN THE TRADE, OR BY COURSE OF DEALING. 
Silver Spring DOES NOT WARRANT THAT THE SOFTWARE OR ANY PORTION THEREOF WILL BE ERROR-FREE.

Termination. This License is effective until terminated. Your rights under this License will terminate automatically without notice if you fail to comply with any of its terms. 
Upon the termination of this License, You must cease all use of the Software and remove it from Your computer.
*/
using System;
using System.Reflection;
using System.Text;
using System.Net;

namespace HdkClient
{
    /// <summary>
    /// Utilities class for various translations, etc.
    /// </summary>
    public static class HdkUtils
    {
        /// <summary>
        /// Verify that the IPAddress is a valid IPV4 address
        /// </summary>
        /// <param name="ipaddr">the IP address to verify</param>
        /// <returns>a boolean indicating whether or not the string is a valid IPV4 address</returns>
        /// 
        private static int __RndStart = 0;
        public static bool IsIPv4Address(string ipaddr)
        {
            IPAddress ip = StringToIPAddress(ipaddr);
            if (ip.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
            {
                return true;
            }

            return false;
        }
        public static IPAddress StringToIPAddress(string str)
        {
            int slash_index = str.IndexOf('/');
            if (slash_index != -1)
            {
                str = str.Substring(0, slash_index);
            }

            return IPAddress.Parse(str);
        }

        /// <summary>
        /// Translate a MAC address to a SSN IPV6 address
        /// </summary>
        /// <param name="mac_bytes">a byte array representing the address</param>
        /// <param name="ipprefix">the prefix for SSN IPV6 addresses</param>
        /// <returns>a byte array representing a valid IPV6 address</returns>
        public static byte[] MacBytesToIpv6(byte[] mac_bytes, byte[] ipprefix)
        {
            byte[] mac_addr = new byte[8];
            if (ipprefix == null)
            {
                throw new Exception("ipprefix cannot be null.");
            }
            if (ipprefix.Length != 8)
            {
                throw new Exception("Expected 8 bytes for ipv6 prefix, got: " +
                    ipprefix.Length);
            }
            if (mac_bytes.Length == 6)
            {
                Array.Copy(mac_bytes, mac_addr, 3);
                mac_addr[3] = 0xFF;
                mac_addr[4] = 0xFE;
                Array.Copy(mac_bytes, 3, mac_addr, 5, 3);
            }
            else if (mac_bytes.Length != 8)
            {
                throw new Exception("Expected 8 bytes for mac bytes, got: " +
                    mac_bytes.Length);
            }
            else
                Array.Copy(mac_bytes, mac_addr, 8);
            byte[] ipv6_bytes = new byte[16];
            Array.Copy(ipprefix, ipv6_bytes, 8);
            Array.Copy(mac_addr, 0, ipv6_bytes, 8, 8);
            ipv6_bytes[8] ^= 0x02;
            return ipv6_bytes;
        }
        /// <summary>
        /// The gateway expects the target of the call to be a DNS resolvable URI.
        /// This function constructs one based on the MAC in the display list.
        /// </summary>
        /// <param name="mac">The MAC to translate.</param>
        /// <returns>The fully-rendered URI</returns>

        public static string UriFromMac(string mac)
        {
            return GatewaySettings.Instance.GatewayDeviceURIPrefix + mac + GatewaySettings.Instance.GatewayDeviceURISuffix;
        }
        /// <summary>
        /// Converts the supplied byte stream into an enhanced human-readable 
        /// string.
        /// </summary>
        /// <param name="bytes">the byte stream to convert</param>
        /// <returns>
        /// a human-readable representation of the supplied byte 
        /// stream
        /// </returns>
        public static string BytesToHexView(byte[] bytes)
        {
            if (bytes == null || bytes.Length == 0)
                return String.Empty;

            return BytesToHexView(bytes, 0, 0, bytes.Length);
        }

        /// <summary>
        /// Converts the supplied byte stream into an enhanced human-readable 
        /// string.
        /// </summary>
        /// <param name="bytes">the byte stream to convert</param>
        /// <param name="max_line_length">the number of bytes to format per line</param>
        /// <returns>
        /// a human-readable representation of the supplied byte 
        /// stream
        /// </returns>
        public static string BytesToHexView(byte[] bytes, int max_line_length)
        {
            if (bytes == null || bytes.Length == 0)
                return String.Empty;

            return BytesToHexView(bytes, 0, 0, bytes.Length, max_line_length);
        }

        /// <summary>
        /// Converts the supplied byte stream into an enhanced human-readable 
        /// string.
        /// </summary>
        /// <param name="bytes">the byte stream to convert</param>
        /// <param name="offset">the the first byte at which to start the conversion</param>
        /// <param name="length">the number of bytes to convert</param>
        /// <returns>
        /// a human-readable representation of the supplied byte 
        /// stream
        /// </returns>
        /// <exception cref="ArgumentException">
        /// if the offset and length parameters extend beyond the boundaries of
        /// the supplied byte array
        /// </exception>
        public static string BytesToHexView(byte[] bytes,
                                            int offset,
                                            int indent)
        {
            if (bytes == null || bytes.Length == 0)
                return String.Empty;

            return BytesToHexView(bytes, offset, indent, bytes.Length);
        }

        /// <summary>
        /// Converts the supplied byte stream into an enhanced human-readable 
        /// string.
        /// </summary>
        /// <param name="bytes">the byte stream to convert</param>
        /// <param name="offset">the the first byte at which to start the conversion</param>
        /// <param name="length">the number of bytes to convert</param>
        /// <param name="indent">the number of spaces to indent each line</param>
        /// <returns>
        /// a human-readable representation of the supplied byte 
        /// stream
        /// </returns>
        /// <exception cref="ArgumentException">
        /// if the offset and length parameters extend beyond the boundaries of
        /// the supplied byte array
        /// </exception>
        public static string BytesToHexView(byte[] bytes,
                                            int offset,
                                            int indent,
                                            int length)
        {
            if (bytes == null || bytes.Length == 0)
                return String.Empty;

            const int max_line_length = 16;
            return BytesToHexView(bytes, offset, indent, bytes.Length, max_line_length);
        }

        /// <summary>
        /// Converts the supplied byte stream into an enhanced human-readable 
        /// string.
        /// </summary>
        /// <param name="bytes">the byte stream to convert</param>
        /// <param name="offset">the the first byte at which to start the conversion</param>
        /// <param name="length">the number of bytes to convert</param>
        /// <param name="indent">the number of spaces to indent each line</param>
        /// <param name="max_line_length">the number of bytes to format per line</param>
        /// <returns>
        /// a human-readable representation of the supplied byte 
        /// stream
        /// </returns>
        /// <exception cref="ArgumentException">
        /// if the offset and length parameters extend beyond the boundaries of
        /// the supplied byte array
        /// </exception>
        public static string BytesToHexView(byte[] bytes,
                                            int offset,
                                            int indent,
                                            int length,
                                            int max_line_length)
        {
            if (bytes == null || bytes.Length == 0)
                return String.Empty;

            int ascii_x_offset = 4 + 2 + max_line_length * 3;

            StringBuilder builder = new StringBuilder();
            int num_lines = (int)System.Math.Ceiling(((double)length / (double)max_line_length));
            for (int line = 0; line < num_lines; ++line)
            {
                builder.Append(' ', indent);
                int line_start = line * max_line_length;
                builder.Append((line_start + offset).ToString("X4") + ": ");
                int remaining_bytes = length - line * max_line_length;
                int line_length = System.Math.Min(max_line_length, remaining_bytes);
                for (int i = 0; i < line_length; ++i)
                {
                    builder.Append(Convert.ToInt32(bytes[line_start + i]).ToString("X2") + " ");
                }

                for (int i = 6 + line_length * 3; i < ascii_x_offset; ++i)
                {
                    builder.Append(" ");
                }

                builder.Append('|');
                builder.Append(' ');
                for (int i = 0; i < line_length; ++i)
                {
                    builder.Append(ToPrintable(bytes[line_start + i]));
                }

                builder.Append("\r\n");
            }

            return builder.ToString(0, builder.Length - 2);
        }
        /// <summary>
        /// Converts the specified byte to a printable character.
        /// </summary>
        /// <param name="b">the byte to convert</param>
        /// <returns>a printable representation of the specified byte</returns>
        public static char ToPrintable(byte b)
        {
            if (b >= 32 && b <= 126)
            {
                return (char)b;
            }

            return '.';
        }

        /// <summary>
        /// Converts a byte array to a printable string.
        /// </summary>
        /// <param name="bytes">the bytes to convert</param>
        /// <returns>a printable representation of the specified array of bytes</returns>
        public static string ToPrintable(byte[] bytes)
        {
            string result = "";
            foreach (byte b in bytes)
            {
                result = result + ToPrintable(b);
            }

            return result;
        }
        /// <summary>
        /// Builds a string of the assembly version as defined in AssemblyInfo.cs
        /// </summary>
        /// <param name="a">the assembly to examine</param>
        /// <returns>a string representing the assembly version</returns>
        public static string GetAssemblyVersion(Assembly a)
        {
            try
            {
                System.Version thisAppVersion = a.GetName().Version;
                return thisAppVersion.Major.ToString()
                + "." + thisAppVersion.Minor.ToString()
                + "." + thisAppVersion.Build.ToString();
                //+ "." + thisAppVersion.Revision.ToString();
            }
            catch
            {
                return "unknown";
            }
        }
        /// <summary>
        /// Starts with a random integer as a seed.
        /// Increments the first random integer after that.
        /// </summary>
        /// <returns>a ushort to be used as a message id</returns>
        public static ushort MessageId()
        {
            if (__RndStart == 0)
            {
                Random r = new Random(DateTime.Now.Second);
                __RndStart = r.Next();
            }
            return (ushort)++__RndStart;
        }
    }
}
