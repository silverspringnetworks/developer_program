using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HdkClient
{
    public class UserToken
    {
        public string Destination { get; set; }
        public DateTime InitiatedTime { get; set; }
        ~UserToken()
        {
        }
    }
}
