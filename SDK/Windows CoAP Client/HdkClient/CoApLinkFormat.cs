using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HdkClient
{
    public  class CoApLinkFormat
    {
        public enum Format
        {
            rt,     // Resource Type
            @if,    // InterFace 
            ct,     // Content Type
            sz,     // max SiZe estimate
            title,  // Title
            obs,    // Observable
            href    // Href/link
        }

     }
}
