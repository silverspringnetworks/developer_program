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

namespace EXILANT.Labs.CoAP.Exceptions
{
    /// <summary>
    /// Exception class that indicates an exception when message goes undelivered.
    /// Not implementing ISerializable since that is not available in .NET micro framework
    /// and we do not intend to marshal exceptions in their serialized format in CoAP
    /// </summary>
    public class UndeliveredException : Exception
    {
        /// <summary>
        /// Default constructor
        /// </summary>
        public UndeliveredException() { }
        /// <summary>
        /// Constructor with an error message
        /// </summary>
        /// <param name="message">Error message associated with the exception</param>
        public UndeliveredException(string message) : base(message) { }
        /// <summary>
        /// Constructor to wrap an exception
        /// </summary>
        /// <param name="message">Error message associated with the exception</param>
        /// <param name="inner">The actual exeption that is being wrapped by this exception</param>
        public UndeliveredException(string message, Exception inner) : base(message, inner) { }
    }
}
