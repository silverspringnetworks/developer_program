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
using System.Net;

namespace EXILANT.Labs.CoAP.Helpers
{
    /// <summary>
    /// A class to provide utility methods to work with the network
    /// </summary>
    public abstract class AbstractNetworkUtils
    {
        /// <summary>
        /// Convert the byte array to network byte order. If the conditional compilation
        /// symbol _BIG_ENDIAN is defined, then it is assumed that the target machine is
        /// big endian else its assumed that the target machine is little endian
        /// </summary>
        /// <param name="data">A byte array</param>
        /// <returns>byte array in network byte order</returns>
        public static byte[] ToNetworkByteOrder(byte[] data)
        {
            if (data == null || data.Length < 2) return data;
            if (AbstractByteUtils.IsTargetLittleEndian())
                data = AbstractByteUtils.ReverseBytes(data);

            return data;
        }
        /// <summary>
        /// Convert the byte array into local representation from network byte order representation
        /// If the conditional compilation symbol _BIG_ENDIAN is defined, then it is assumed 
        /// that the target machine is big endian else its assumed that the target machine 
        /// is little endian
        /// </summary>
        /// <param name="data">A byte array</param>
        /// <returns>byte array in local byte order</returns>
        public static byte[] FromNetworkByteOrder(byte[] data)
        {
            if (data == null || data.Length < 2) return data;
            if (AbstractByteUtils.IsTargetLittleEndian())
                data = AbstractByteUtils.ReverseBytes(data);
            return data;
        }
        
        /// <summary>
        /// Get the default CoAP port address
        /// </summary>
        /// <returns>UInt16</returns>
        public static UInt16 GetDefaultCoAPPort()
        {
            return 5683;
        }
        /// <summary>
        /// Get the maximum size of the message in bytes. This includes header, token, options and payload 
        /// The current value is set at 2KB bytes. 
        /// For anything bigger than this, please look at Block option
        /// </summary>
        /// <returns>UInt16</returns>
        public static UInt16 GetMaxMessageSize()
        {            
            return 2048;
        }
        /// <summary>
        /// Get the IP address from hostname
        /// </summary>
        /// <param name="hostname">The hostname whose IP address is required</param>
        /// <returns>IPAddress</returns>
        public static IPAddress GetIPAddressFromHostname(string hostname)
        {
            IPAddress ipAddr = null;
            bool strRepresentsIPAddr = false;
            try
            {
                ipAddr = IPAddress.Parse(hostname);
                strRepresentsIPAddr = true;
            }
            catch
            {
                ; ;//String does not represent an IP address
            }
            if (!strRepresentsIPAddr)
            {
                IPHostEntry ipe = Dns.GetHostEntry(hostname);
                foreach (IPAddress ipAdrrInList in ipe.AddressList)
                {
                    if (ipAdrrInList != null)
                    {
                        ipAddr = ipAdrrInList;
                        break;
                    }
                }
            }
            return ipAddr;
        }
    }
}
