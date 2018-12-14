using System;

namespace HdkClient
{
    public class ClientRunOptions
    {
        public enum NetworkOption
        {
            UseLAN,
            UseGateway,
            UseFSU
        }

        private static NetworkOption __Using; 
        public static NetworkOption Using
        {
            get { return __Using; }
            set { __Using = value; }
        }
        public static bool UsingLAN
        {
            get { return __Using == NetworkOption.UseLAN; }
        }
        public static bool UsingFSU
        {
            get { return __Using == NetworkOption.UseFSU; }
        }
        public static bool UsingGateway
        {
            get { return __Using == NetworkOption.UseGateway; }
        }
    }
}
