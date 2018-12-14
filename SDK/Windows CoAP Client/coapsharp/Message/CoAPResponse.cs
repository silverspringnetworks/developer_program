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
    /// Represents a standard response object in CoAP exchange
    /// </summary>
    public class CoAPResponse : AbstractCoAPMessage
    {
        #region Constructors
        /// <summary>
        /// Default constructor
        /// </summary>
        public CoAPResponse() { this.Version = new CoAPVersion(); this.Options = new CoAPHeaderOptions(); }
        /// <summary>
        /// The constructor
        /// </summary>
        /// <param name="mType">The message type</param>
        /// <param name="mCode">The message code</param>
        /// <param name="id">The message Id</param>
        public CoAPResponse(byte mType, byte mCode, UInt16 id)
        {
            if (mType == CoAPMessageType.CON)
                throw new ArgumentException("A response message must be of type ACK or RST or NON");

            if (!CoAPMessageCode.DoesMessageCodeRepresentAResponse(mCode))
                throw new ArgumentException("The message code can only be used for a request");

            this.Version = new CoAPVersion();

            this.MessageType = new CoAPMessageType();
            this.MessageType.Value = mType;

            this.Code = new CoAPMessageCode();
            if (!this.Code.IsValid(mCode))
                throw new ArgumentException("Invalid message code in request");
            this.Code.Value = mCode;
            this.ID = new CoAPMessageID(id);
            this.Options = new CoAPHeaderOptions();
        }
        /// <summary>
        /// The constructor
        /// </summary>
        /// <param name="mType">The message type</param>
        /// <param name="mCode">The message code</param>
        /// <param name="coapReq">The CoAPRequest against which this response is being constructed</param>
        public CoAPResponse(byte mType, byte mCode, CoAPRequest coapReq)
        {
            if (mType == CoAPMessageType.CON)
                throw new ArgumentException("A response message must be of type ACK or RST or NON");

            if (!CoAPMessageCode.DoesMessageCodeRepresentAResponse(mCode))
                throw new ArgumentException("The message code can only be used for a request");

            this.Version = new CoAPVersion();

            this.MessageType = new CoAPMessageType();
            this.MessageType.Value = mType;

            this.Code = new CoAPMessageCode();
            if (!this.Code.IsValid(mCode) )
                throw new ArgumentException("Response code invalid");
            this.Code.Value = mCode;
            this.ID = new CoAPMessageID(coapReq.ID.Value);
            this.Token = new CoAPToken(coapReq.Token.Value);
            //TOCHECK::this.Options = new CoAPHeaderOptions(coapReq.Options);
            this.Options = new CoAPHeaderOptions();
            //Other needed parameters
            this.RemoteSender = coapReq.RemoteSender;
        }
        #endregion

        #region Operations
        /// <summary>
        /// Set the location URL. This is set by the response to indicate "Created" result if the
        /// request is POST (to create a new resource)
        /// </summary>
        /// <param name="locationURL">The location URL relative to the URI that got created</param>
        public void SetLocation(string locationURL)
        {
            if (locationURL == null || locationURL.Trim().Length == 0) throw new ArgumentException("Invalid CoAP location URL");
            locationURL = locationURL.Trim().ToLower();

            if (locationURL.IndexOf("#") >= 0) throw new ArgumentException("Fragments not allowed in CoAP location URL");
            //Add these items as option
                        
            //Path components
            string[] segments = AbstractURIUtils.GetUriSegments(locationURL);

            if (segments != null && segments.Length > 0)
            {
                foreach (string segment in segments)
                {
                    if (segment.Trim().Length == 0) continue;
                    this.Options.AddOption(CoAPHeaderOption.LOCATION_PATH, AbstractByteUtils.StringToByteUTF8(AbstractURIUtils.UrlDecode(segment)));
                }
            }
            //Query
            string[] qParams = AbstractURIUtils.GetQueryParameters(locationURL);
            if (qParams != null && qParams.Length > 0)
            {
                foreach (string queryComponent in qParams)
                {
                    if (queryComponent.Trim().Length == 0) continue;
                    this.Options.AddOption(CoAPHeaderOption.LOCATION_QUERY, AbstractByteUtils.StringToByteUTF8(AbstractURIUtils.UrlDecode(queryComponent)));
                }
            }
        }
        #endregion
    }
}
