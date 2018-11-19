using System;
using System.Net;

namespace EXILANT.Labs.CoAP.Settings
{
    public class CoAPSettings
    {
        private static CoAPSettings __Instance = null;

        private static bool __Confirmable = false;
        private System.Net.Sockets.AddressFamily __AddressFamily;
        public static CoAPSettings Instance
        {
            get
            {
                if (__Instance == null)
                {
                    __Instance = new CoAPSettings();
                }

                return __Instance;
            }
        }
        public System.Net.Sockets.AddressFamily AddressFamily
        {
            get { return __AddressFamily; }
            set { __AddressFamily = value; }
        }

        public bool Confirmable
        {
            get { return __Confirmable; }
            set { __Confirmable = value; }
        }
        public bool NonConfirmable
        {
            get { return !__Confirmable; }
            set { __Confirmable = !value; }
        }

    }
}
