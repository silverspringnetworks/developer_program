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
    /// CoAPSharp requires a logger class to work. If you forget to implement a logger, this
    /// library will fail. That is not good :-)
    /// Therefore, this default logger class is made to ensure, that if you forget to 
    /// provide your own logging class, then nothing fails, but all logging calls are
    /// silently ignored.
    /// You can argue that we could have used the "Debug" class from SPOT namespace, but
    /// we would like to keep the entire library free from any such namespace
    /// </summary>
    public class DefaultLogger : AbstractLogUtil
    {
        /// <summary>
        /// The only method that is required to be implemented.
        /// This mthod is a dummy implementation and thus all
        /// calls simply do nothing
        /// </summary>
        /// <param name="level">The log level</param>
        /// <param name="message">The message to log</param>
        protected override void LogIt(LogLevel level, string message)
        {
            //No Work :-)
        }
    }
}
