using System;
using System.Threading;
using CattEng;
using NetMgr;
using EXILANT.Labs.CoAP.Settings;

namespace HdkClient
{
    public class CoApGetFsu : CoApGet
    {
        public override void Send()
        {
            CoAPSettings.Instance.AddressFamily = System.Net.Sockets.AddressFamily.InterNetworkV6;
            byte[] payload = this.ToBytes();
            TestStatus result = new TestStatus();
            TestManager m_test_manager = new TestManager();
            BasicReporter r = new BasicReporter(result);
            m_test_manager.TestReporter = r;

            CoApFsuCommand cmd = new CoApFsuCommand();
            cmd.CoApPayload = this.ToBytes();
            FsuFacade.UseSecurePort = false;
            FsuFacade.UseNullHeader = false;
            FsuFacade.RequiresUserPIN = false;
            FileLogger.Write(String.Format("FsuFacade.UseSecurePort={0}", FsuFacade.UseSecurePort));
            FileLogger.Write(String.Format("FsuFacade.UseNullHeader={0}", FsuFacade.UseNullHeader));
            FileLogger.Write(String.Format("FsuFacade.RequiresUserPIN={0}", FsuFacade.RequiresUserPIN));
            CattEng.Test test = cmd.ToTest(this.IpAddress);//);SSNUtils.MiscTypes.fsu_ip_address);

            m_test_manager.Add(test);
            DateTime startTime = DateTime.Now;
            while (!m_test_manager.Empty)
            {
                Console.WriteLine("Sleeping");
                Thread.Sleep(50);
                DateTime now = DateTime.Now;
                TimeSpan diff = now - startTime;
                if (diff.TotalMilliseconds > CoApSettings.Instance.RequestTimeout)
                {
                    m_test_manager.Clear();
                    break;
                }
            }

            CoApFsuTest t = (CoApFsuTest)r.TestResult;
            FileLogger.Write(String.Format("Fsu call returned {0}", r.TestResult.CurStatus));
            try
            {
                CoApFsuResponse resp = (CoApFsuResponse)r.TestResult.ResponseResult.response;
                byte[] respPayload = resp.Payload;

                //CoAPResponse cr = new CoAPResponse();
                //cr.FromByteStream(respPayload);
                //CoAPPayload p = cr.Payload;
                base.SetGetResult(resp.Payload);
                string toLog = SSNUtils.Conversion.BytesToHexView(resp.Payload);
                FileLogger.Write(String.Format("Payload returned:\n {0}", toLog));
            }
            catch
            {
                base.SetGetResult(new byte[0]);
            }
            //(p.ToStream(0));        }
        }
    }
}
