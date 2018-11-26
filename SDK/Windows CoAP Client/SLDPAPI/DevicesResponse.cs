using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SLDPAPI
{
    public class DevicesResponse

    {
        public List<Device> devices { get; set; }
        public class DomainInfo
        {
            public string nic_macID { get; set; }
        }

        public class Device
        {
            public string id { get; set; }
            public string deviceType { get; set; }
            public DomainInfo domainInfo { get; set; }
        }
    }
}
