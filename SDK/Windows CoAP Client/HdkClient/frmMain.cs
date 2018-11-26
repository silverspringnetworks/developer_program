using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using EXILANT.Labs.CoAP.Channels;
using EXILANT.Labs.CoAP.Message;
using System.Threading;
using EXILANT.Labs.CoAP.Helpers;
using coapsharp.Properties;
using SSNUtils;
using System.Net;
using CattEng;
using NetMgr;
using System.Diagnostics;
using System.Net.NetworkInformation;
using SLDPAPI;
using Newtonsoft.Json;
using System.Reflection;

namespace HdkClient
{
    public partial class frmMain : Form
    {
        AutoResetEvent __Done = new AutoResetEvent(false);
        AutoResetEvent __DonePing = new AutoResetEvent(false);
        AutoResetEvent __DoneFsuNodeQ = new AutoResetEvent(false);

        public frmMain()
        {
            InitializeComponent();
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            lstDevices.Items.Clear();
            lstDevices.Refresh();
        }

        private void frmMain_Load(object sender, EventArgs e)
        {
            cboPort.SelectedIndex = 0;
            FsuInit i = new FsuInit();
            i.DoneWaiting = __Done;
            ThreadStart threadDelegate = new ThreadStart(i.Doit);
            Thread newThread = new Thread(threadDelegate);
            newThread.IsBackground = true;
            newThread.Start();
            FileLogger.Enabled = false;
            //pictureBox1.SizeMode = PictureBoxSizeMode.AutoSize;
        }

        private void btnRefresh_MouseHover(object sender, EventArgs e)
        {
            ctrlTip.SetToolTip(btnRefresh, "Refresh");
        }

        private void btnRun_MouseHover(object sender, EventArgs e)
        {
            ctrlTip.SetToolTip(btnRun, "Execute");
        }

        private void frmMain_Resize(object sender, EventArgs e)
        {
            //lstDevices.Height = this.Height * __DeviceListHeightPercent / 100;
        }

        private void lstDevices_MouseHover(object sender, EventArgs e)
        {
            ctrlTip.SetToolTip(lstDevices, "Reachable CoAP devices");
        }
        private void lstDevices_Click(object sender, EventArgs e)
        {
            if (lstDevices.Items.Count == 0)
                return;
            ListBox box = (ListBox)sender;
            if (box.SelectedItem == null)
                return;
            string ipAddr = box.SelectedItem.ToString();
        }

        private void btnRun_Click(object sender, EventArgs e)
        {

        }

        private void frmMain_FormClosing(object sender, FormClosingEventArgs e)
        {
        }

        private void ClearAll()
        {
            lstDevices.Items.Clear();
            lstDevices.Refresh();
            treeResources.Nodes.Clear();
            txtResult.Clear();
        }
        private void btnImportIpAddresses_Click(object sender, EventArgs e)
        {
            ClearAll();
            IpAddressLoader l = new IpAddressLoader();
            l.Load();
        }

        private void radLAN_CheckedChanged(object sender, EventArgs e)
        {
            ClearAll();
            NetworkInterface.Instance.SelectedInterface = NetworkInterface.NetworkOption.LAN;
        }

        private void radGateway_CheckedChanged(object sender, EventArgs e)
        {
            ClearAll();
            NetworkInterface.Instance.SelectedInterface = NetworkInterface.NetworkOption.Gateway;
            LoadNodes();
        }

        private void radFSU_CheckedChanged(object sender, EventArgs e)
        {
            ClearAll();
            NetworkInterface.Instance.SelectedInterface = NetworkInterface.NetworkOption.Fsu;

            // We only want to set the default FSU if we are switching to FSU mode.
            if (!radFSU.Checked)
            {
                radFSU.Text = "FSU (none)";
                return;
            }

            SetFSU();

            LoadNodes();

        }

        private void SetFSU()
        {
            // Assign the default FSU.
            try
            {
                Cursor.Current = Cursors.WaitCursor;
                if (__Done != null)
                {
                    __Done.WaitOne();
                }
                string fsuComPort = FsuFacade.Instance.DefaultFsuInterface;
                if (FsuFacade.Instance.DefaultFsuInterface == null)
                {
                    fsuComPort = "none found";
                }
                radFSU.Text = String.Format("FSU ({0})", fsuComPort);
                if (__Done != null)
                {
                    __Done.Close();
                    __Done.Dispose();
                    __Done = null;
                }
            }
            catch (Exception fsuError)
            {
                MessageBox.Show(fsuError.Message);
            }
            finally
            {
                Cursor.Current = Cursors.Default;
            }

        }

        private void LoadNodes()
        {
            CoApDeviceFinder finder = (CoApDeviceFinder)NetworkInterface.Instance.NodeFinder;
            CoApDevices devices = finder.LoadNodes();
            foreach (CoApDevice d in devices)
            {
                lstDevices.Items.Add(d.Name);
            }
        }
        private void treeResources_MouseHover(object sender, EventArgs e)
        {
            ctrlTip.SetToolTip(treeResources, "Resources available for selected CoAP device");
        }

        private void treeResources_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            
            Cursor.Current = Cursors.WaitCursor;
            CoApGet get = new CoApGet();
            get.IpAddress = lstDevices.SelectedItem.ToString();
            get.ServerPort = Convert.ToInt32(cboPort.Text);
            get.URI = treeResources.SelectedNode.Text;
            if (NetworkInterface.Instance.UsingFSU)
            {
                byte[] payload = get.ToBytes();
                TestStatus result = new TestStatus();
                TestManager m_test_manager = new TestManager();
                BasicReporter r = new BasicReporter(result);
                m_test_manager.TestReporter = r;

                CoApFsuCommand cmd = new CoApFsuCommand();
                cmd.CoApPayload = get.ToBytes();
                FsuFacade.UseSecurePort = false;
                FsuFacade.UseNullHeader = false;
                FsuFacade.RequiresUserPIN = false;
                FileLogger.Write(String.Format("FsuFacade.UseSecurePort={0}", FsuFacade.UseSecurePort));
                FileLogger.Write(String.Format("FsuFacade.UseNullHeader={0}", FsuFacade.UseNullHeader));
                FileLogger.Write(String.Format("FsuFacade.RequiresUserPIN={0}", FsuFacade.RequiresUserPIN));
                CattEng.Test test = cmd.ToTest(lstDevices.SelectedItem.ToString());//);SSNUtils.MiscTypes.fsu_ip_address);

                m_test_manager.Add(test);
                while (!m_test_manager.Empty)
                {
                    Console.WriteLine("Sleeping");
                    Thread.Sleep(50);
                }

                CoApFsuTest t = (CoApFsuTest)r.TestResult;
                CoApFsuResponse resp = (CoApFsuResponse)r.TestResult.ResponseResult.response;
                byte[] respPayload = resp.Payload;
                //CoAPResponse cr = new CoAPResponse();
                //cr.FromByteStream(respPayload);
                //CoAPPayload p = cr.Payload;
                get.SetGetResult(resp.Payload);//(p.ToStream(0));

            }
            else
            {
                get.Send();
            }
            txtResult.Text = get.Response.ToString();
            Cursor.Current = Cursors.Default;
        }

        private void lstDevices_DoubleClick(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            // Ignore items that are currently unreachable.
            if (lstDevices.SelectedItem.ToString().Contains("UNREACHABLE") || lstDevices.SelectedItem.ToString().Trim() == "")
            {
                return;
            }
            CoApDiscovery disc = (CoApDiscovery)NetworkInterface.Instance.DiscoveryRequest;
            disc.ServerPort = Convert.ToInt32(cboPort.Text);
            disc.IpAddress = lstDevices.SelectedItem.ToString();
            CoApResources resources = disc.LoadResources();

            foreach (CoApResource res in resources)
            {
                TreeNode resNode = treeResources.Nodes.Add(res.URI);
                foreach (CoApLinkAttribute a in res.LinkAttributes)
                {
                    TreeNode[] node = new TreeNode[1];
                    node[0] = new TreeNode(a.ToString());
                    resNode.Nodes.AddRange(node);
                }
            }
            disc = null;
            GC.Collect();
            Cursor.Current = Cursors.Default;
            ////////string blat = Serialization.SerializeObject(response);
            ////////blat = Serialization.JSONSerialize(response);
            ////////txtResult.Text = blat;
        }

        private void frmMain_Shown(object sender, EventArgs e)
        {
            radFSU.Checked = false;
            //radFSU.PerformClick();
        }

        private static Task<PingerReply> PingIt(string ipAddr)
        {
            return Task.Run(() => { Pinger p = new Pinger(ipAddr); return p.PingIpAddress(); });
        }
        private void button1_Click(object sender, EventArgs e)
        {
            GetResources("001350050005E7D6");

            object o = NetworkInterface.Instance.DiscoveryRequest;
            //object o = (CoApDiscovery)Activator.CreateInstance(
            //CoApDiscovery d = (CoApDiscovery)Activator.CreateInstance(Type.GetType(thisNamespace + "." + "CoApDiscoveryFsu"));
            return;

            DevicesResponseJSON devices;
            Cursor.Current = Cursors.WaitCursor;
            if (NetworkInterface.Instance.UsingGateway)
            {
                // GET DEVICES WORKS
                devices = GetDevices();
                for (int i = 0; i < devices.queryResults.Count; i++)
                {
                    lstDevices.Items.Add(devices.queryResults[i].nic_macId);
                }
                // PING WORKS
                string res = PingMac(devices.queryResults[0].nic_macId);
                GetResources("001350050005E7D6");
            }
            //coap://SSN001350050005E7D6.SG.PROD.STAR.SSNSGS.NET:5683/.well-known/core

            //The Poster implementation below generates a token correctly.
            //////Poster p = new Poster();
            //////p.GetToken("DY8WxUI4oveZU2sWu3dkoNjCkEUB1Tsh", "vgWI2nZ1rQIS3Acm");
            //////txtResult.Text = p.Token;
            Cursor.Current = Cursors.Default;
            //SLDPAPI.SilverLink link = new SilverLink();
            //link.ClientID = "DY8WxUI4oveZU2sWu3dkoNjCkEUB1Tsh";
            //link.Secret = "vgWI2nZ1rQIS3Acm";
            ////link.BaseURL = "https://api.silverlink.silverspringnet.com/v2/streamingintervals/starfishstaging";
            //link.BaseURL = "https://api.silverlink.silverspringnet.com";
            //link.DeviceSearch(0, 100);
            //Cursor.Current = Cursors.Default;
            //CoApPut put = new CoApPut();
            //put.IpAddress = lstDevices.SelectedItem.ToString();
            //put.ServerPort = Convert.ToInt32(cboPort.Text);
            //put.URI = treeResources.SelectedNode.Text;
            //put.Send();
            //txtResult.Text = put.PutResult;
            ////frmDebug d= new frmDebug();
            ////d.ShowDialog();
            ////string blat = FileLogger.Level.ToString();
            ////FileLogger.Write("Test", FileLogger.LogLevel.Debug);
            //////treeView1.Nodes.Clear();
            //////treeView1.Nodes.Add("10.90.202.182", "10.90.202.182");
            //////treeView1.Nodes.Add("10.90.202.220", "10.90.202.220");
            //////int ix = treeView1.Nodes.IndexOfKey("10.90.202.182");
            ////////treeView1.Nodes[ix].BackColor = SystemColors.GradientInactiveCaption;
            //////treeView1.Nodes[ix].ForeColor = SystemColors.InactiveCaption;

            //////treeView1.LineColor = SystemColors.InactiveBorder;
        }
        public DevicesResponseJSON GetDevices()
        {
            var model = new DevicesRequestJSON()
            {
                docTemplate = new List<string>
                {
"*"           },
                pageNumber = 1,
                pageSize = 50
            };
            //00:13:50:05:00:11:90:e6
            string jsonBody = JsonConvert.SerializeObject(model);

            SLDPAPI.SilverLink link = new SilverLink();
            link.ClientID = "u9p7PiX9b35jgZuQAxqsMw1tfcRSZKTA";//Chieh-min="u9p7PiX9b35jgZuQAxqsMw1tfcRSZKTA";// Mike's="DY8WxUI4oveZU2sWu3dkoNjCkEUB1Tsh";
            link.Secret = "ZY3GjWNDWMI67ASH"; //Chieh-min="ZY3GjWNDWMI67ASH";// Mike's="vgWI2nZ1rQIS3Acm";
            link.CustomerID = "starfishstaging";
            string CustomerID = "starfishstaging";
            //link.BaseURL = "https://api.silverlink.silverspringnet.com/v2/streamingintervals/starfishstaging";//https://api.silverlink.silverspringnet.com/sdp/v1/ping/starfishstaging?nic_macId=001350050005E7D6
            link.BaseURL = "https://api.silverlink.silverspringnet.com";

            var json = link.Post("/sdp/v1/devices/" + CustomerID + "?op=SEARCH", CustomerID, jsonBody);
            var devices = JsonConvert.DeserializeObject<DevicesResponseJSON>(json);
            return devices;
            //var datasetsJSON = JsonConvert.DeserializeObject<DatasetJSONModel>(json);

            //Assert.IsTrue(datasetsJSON.Datasets.Any());
        }

        public string GetResources(string mac)
        {
            var model = new DevicesRequestJSON()
            {
                docTemplate = new List<string>
                {
"*"           },
                pageNumber = 1,
                pageSize = 50
            };
            //00:13:50:05:00:11:90:e6
            string jsonBody = JsonConvert.SerializeObject(model);

            SLDPAPI.SilverLink link = new SilverLink();
            link.ClientID = "u9p7PiX9b35jgZuQAxqsMw1tfcRSZKTA";//Chieh-min="u9p7PiX9b35jgZuQAxqsMw1tfcRSZKTA";// Mike's="DY8WxUI4oveZU2sWu3dkoNjCkEUB1Tsh";
            link.Secret = "ZY3GjWNDWMI67ASH"; //Chieh-min="ZY3GjWNDWMI67ASH";// Mike's="vgWI2nZ1rQIS3Acm";
            link.CustomerID = "starfishstaging";
            string CustomerID = "starfishstaging";
            //link.BaseURL = "https://api.silverlink.silverspringnet.com/v2/streamingintervals/starfishstaging";//https://api.silverlink.silverspringnet.com/sdp/v1/ping/starfishstaging?nic_macId=001350050005E7D6
            link.BaseURL = "https://api.silverlink.silverspringnet.com";
            // GET https://api.silverlink.silverspringnet.com/sdp/v1/webhook/hke?nic_macid=00beef1f000f051d

            string resp = link.Get("/sdp/v1/webhook/" + CustomerID + "?nic_macid=" + mac, CustomerID);
            return resp;
        }

        public string PingMac(string mac)
        {
            var model = new DevicesRequestJSON()
            {
                docTemplate = new List<string>
                {
"*"           },
                pageNumber = 1,
                pageSize = 50
            };
            //00:13:50:05:00:11:90:e6
            string jsonBody = JsonConvert.SerializeObject(model);

            SLDPAPI.SilverLink link = new SilverLink();
            link.ClientID = "u9p7PiX9b35jgZuQAxqsMw1tfcRSZKTA";//Chieh-min="u9p7PiX9b35jgZuQAxqsMw1tfcRSZKTA";// Mike's="DY8WxUI4oveZU2sWu3dkoNjCkEUB1Tsh";
            link.Secret = "ZY3GjWNDWMI67ASH"; //Chieh-min="ZY3GjWNDWMI67ASH";// Mike's="vgWI2nZ1rQIS3Acm";
            link.CustomerID = "starfishstaging";
            string CustomerID = "starfishstaging";
            //link.BaseURL = "https://api.silverlink.silverspringnet.com/v2/streamingintervals/starfishstaging";//https://api.silverlink.silverspringnet.com/sdp/v1/ping/starfishstaging?nic_macId=001350050005E7D6
            link.BaseURL = "https://api.silverlink.silverspringnet.com";
            // GET https://api.silverlink.silverspringnet.com/sdp/v1/webhook/hke?nic_macid=00beef1f000f051d

            string resp = link.Get("/sdp/v1/ping/" + CustomerID + "?nic_macid=" + mac, CustomerID);
            return resp;
        }

        //https://api.silverlink.silverspringnet.com/sdp/v1/ping/starfishstaging?&nic_macId=001350050005E7D6 Authorization: Bearer 2u0vpUXsA2v8oC744jIsTmKI0kHG

        private void txtTimeout_TextChanged(object sender, EventArgs e)
        {
            try
            {
                int x = Convert.ToInt32(txtTimeout.Text);
            }
            catch
            {
                //MessageBox.Show("Invalid image delete retries");
                txtTimeout.SelectAll();
                txtTimeout.Focus();
                return;
            }
        }

        private void btnDebug_Click(object sender, EventArgs e)
        {
            frmDebug d = new frmDebug();
            d.ShowDialog();
        }
    }
}
