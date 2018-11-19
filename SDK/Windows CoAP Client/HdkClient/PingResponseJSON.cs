using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using EXILANT.Labs.CoAP.Helpers;

namespace HdkClient
{
    /// <summary>
    /// Parse JSON ping response from Gateway
    /// </summary>
    public class PingResponseJSON
    {
        private string __Response = "";
        public PingResponseJSON(string resp)
        {
            __Response = resp;
        }
        /// <summary>
        /// Returns a boolean indicating whether or not the ping worked.
        /// </summary>
        public bool Succeeded
        {
            get
            {
                int found = __Response.IndexOf("SUCCEEDED");
                if (found >= 0)
                    return true;

                return false;
            }
        }
    }
}
