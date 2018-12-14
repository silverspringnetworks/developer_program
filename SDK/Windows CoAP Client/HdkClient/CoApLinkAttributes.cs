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
using System.Collections;

using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HdkClient
{
    /// <summary>
    /// Collection of all link attributes related to a resource.
    /// </summary>
    public class CoApLinkAttributes : CollectionBase

    {
        /// <summary>
        /// Default constructor
        /// </summary>
        public CoApLinkAttributes()
        { }
        /// <summary>
        /// Constructor that contains a complete list of resource attributes.
        /// </summary>
        /// <param name="resource">resource string to be parsed into attributes</param>
        public CoApLinkAttributes(string resource)
        {
            // Split the string by the semicolon character
            string[] linkAttributes = resource.Split(';');
            // The first attribute is actually the resource name, so we ignore it.
            for (int i = 1; i < linkAttributes.Length; i++)
            {
                //Create a link attribute object and add it to our list.
                CoApLinkAttribute a = new CoApLinkAttribute(linkAttributes[i]);
                this.InnerList.Add(a);
            }
        }

        /// <summary>
        /// Referencer that returns the nth attribute in the list.
        /// </summary>
        /// <param name="index"></param>
        /// <returns></returns>
        public CoApLinkAttribute this[int index]
        {
            get
            {
                return ((CoApLinkAttribute)InnerList[index]);
            }
            set
            {
                InnerList[index] = value;
            }
        }

        /// <summary>
        /// Implementation of the Add collection function.
        /// </summary>
        /// <param name="value"></param>
        /// <returns></returns>
        public int Add(CoApLinkAttribute value)
        {
            return (InnerList.Add(value));
        }

    }
}
