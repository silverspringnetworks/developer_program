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
using System.Drawing;
using System.Windows.Forms;
using System.IO;
using EXILANT.Labs.CoAP.Settings;

namespace HdkClient
{
    /// <summary>
    /// This is the main form for the application.
    /// All devices, resources, and features are accessible from this form.
    /// </summary>
    public partial class frmNewMain : Form
    {
        private int __EdgeWidth = 0;
        private int __PreferredResultsHeight = 0;
        private CoApObserverGateway __Observer = null;
        private bool __LoadResources = false;

        public frmNewMain()
        {
            InitializeComponent();
        }
        /// <summary>
        /// Initial form load.
        /// Set all default parameters and 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void frmNewMain_Load(object sender, EventArgs e)
        {
            // Set various client connection settings.
            GatewaySettings.Instance.Restore();

            // Set up the debug log
            SetDebugLogPath();
            FileLogger.Enabled = true;
            FileLogger.Level = FileLogger.LogLevel.Debug;
            FileLogger.Write("Starting HDK Client");
            FileLogger.DebugWindow = txtLog;

            // Display the options panel to the right of the main form.
            __EdgeWidth = pnlEdge.Right - pnlOptions.Right;
            mnuFunctions.BringToFront();
            mnuFunctions.Visible = false;

            //radAutoDiscover.Checked = true;
            radDemandDiscover.Checked = true;
            NetworkInterface.Instance.SelectedInterface = NetworkInterface.NetworkOption.Gateway;
            radGateway.Checked = true;
            radConfirmable.Enabled = true;
            radConfirmable.Checked = true;
            radNonConfirmable.Enabled = true;
            radNonConfirmable.Checked = false;

            __PreferredResultsHeight = pnlResults.Height;
            txtLog.Width = treeDevices.Width + pnlOptions.Width;

            cboPort.SelectedIndex = 0;
            txtTimeout.Text = Convert.ToString(GatewaySettings.Instance.RequestTimeout / 1000);
        }
        /// <summary>
        /// Make sure that the application creates a debug log in a location
        /// that is accessible to the application.
        /// </summary>
        private void SetDebugLogPath()
        {
            string userDataPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData),
                Application.CompanyName + Path.DirectorySeparatorChar
                + "Starfish HDK Client" + Path.DirectorySeparatorChar);
            if (!Directory.Exists(userDataPath))
            {
                Directory.CreateDirectory(userDataPath);
            }
            FileLogger.Path = userDataPath + "ClientDebugLog.txt";
        }
        /// <summary>
        /// Captures the new right-most edge value for the main form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void frmNewMain_ResizeEnd(object sender, EventArgs e)
        {
            __EdgeWidth = pnlEdge.Width;
        }
        /// <summary>
        /// Display the options side of the form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void mnuOptions_Click(object sender, EventArgs e)
        {
            // Toggle the visibility of the options panel.
            pnlOptions.Visible = !pnlOptions.Visible;

            // Depending on the visiblity of the options panel, adjust the edge of the main form.
            if (!pnlOptions.Visible)
            {
                this.Width = this.Width - pnlOptions.Width - __EdgeWidth;
                treeDevices.Width = treeDevices.Width + pnlOptions.Width;
                txtLog.Width = treeDevices.Width;
            }
            else
            {
                this.Width = this.Width + pnlOptions.Width + __EdgeWidth;
                treeDevices.Width = treeDevices.Width - pnlOptions.Width;
                mnuFunctions.Visible = false;
                txtLog.Width = treeDevices.Width + pnlOptions.Width;
            }
        }
        // Handle right-click event to display addtional options.
        private void treeDevices_MouseDown(object sender, MouseEventArgs e)
        {
            // Ignore left-click
            if (e.Button == MouseButtons.Right)
            {
                mnuFunctions.Top = mnuFunctions.Location.X + e.Location.X;
                mnuFunctions.Left = mnuFunctions.Location.Y + e.Location.Y;
                mnuFunctions.Location = new Point(e.X, e.Y + (mnuFunctions.Height / 2));
                mnuFunctions.Visible = true;
            }
        }

        // If the close button is clicked on the options panel, we want to make it invisible 
        // and readjust the main form size.
        private void btnCloseOptions_Click(object sender, EventArgs e)
        {
            this.Width = this.Width - pnlOptions.Width - __EdgeWidth;
            treeDevices.Width = treeDevices.Width + pnlOptions.Width;
            mnuFunctions.Visible = false;
            pnlOptions.Visible = false;
            txtLog.Width = treeDevices.Width;
        }
        /// <summary>
        /// The Debug option has been selected.
        /// Display the Debug form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void mnuDebug_Click(object sender, EventArgs e)
        {
            frmDebug d = new frmDebug();
            d.StartPosition = FormStartPosition.CenterParent;
            d.ShowDialog();
            mnuFunctions.Hide();
        }
        /// <summary>
        /// Clean up UI.  
        /// Clear the nodes previously loaded, reset some settings.
        /// </summary>
        private void ClearAll()
        {
            treeDevices.Nodes.Clear();
            txtResult.Clear();
            radConfirmable.Enabled = true;
            radNonConfirmable.Enabled = true;
            cboPort.SelectedIndex = 0; 
        }

        /// <summary>
        /// Load devices based on the network interface settings.
        /// For each valid device, load a list of related CoAP resources.
        /// </summary>
        private void LoadNodes()
        {
            Cursor.Current = Cursors.WaitCursor;    // Show as buys while loading the resources.

            FileLogger.Write("Loading all known devices");

            // Load the correct device class based on current network option
            CoApDeviceFinder finder = (CoApDeviceFinder)NetworkInterface.Instance.NodeFinder;
            CoApDevices devices = new CoApDevices();
            try
            {
                // With the Ping change, we are capturing the gateway credentials earlier than we had planned.
                //SetGatewayCoapApiCredentials();
                devices = finder.LoadNodes();
            }
            catch (Exception devFail)
            {
                MessageBox.Show("Device fetch failed - " + devFail.Message);
                return;
            }

            // Loop through all fetched devices.
            // If the device was successfully pinged, then we can load related resources.
            // Otherwise, we ignore it.
            foreach (CoApDevice d in devices)
            {
                TreeNode deviceNode = treeDevices.Nodes.Add(d.Name);
                if (d.Reachable)
                {
                    deviceNode.ForeColor = treeDevices.ForeColor;
                    if (NetworkInterface.Instance.UsingGateway)
                    {
                        CoAPSettings.Instance.AddressFamily = System.Net.Sockets.AddressFamily.InterNetwork;
                    }
                    if (NetworkInterface.Instance.UsingSimulator)
                    { 
                        CoAPSettings.Instance.AddressFamily = System.Net.Sockets.AddressFamily.InterNetworkV6;
                    }
                    // We should never get here, but this is added in case we add other communication methods later.
                    if (!NetworkInterface.Instance.UsingGateway && !NetworkInterface.Instance.UsingSimulator)
                    {
                        if (HdkUtils.IsIPv4Address(d.Name))
                        {
                            CoAPSettings.Instance.AddressFamily = System.Net.Sockets.AddressFamily.InterNetwork;
                        }
                        else
                        {
                            CoAPSettings.Instance.AddressFamily = System.Net.Sockets.AddressFamily.InterNetworkV6;
                        }
                    }

                    // We need to load the credentials if they have not already been set.
                    bool credentialsOk = true;
                    //if (NetworkInterface.Instance.UsingGateway)
                    //{
                    //    credentialsOk = SetGatewayCoapApiCredentials();
                    //}

                    // Check to see if we are ready to load resources related to a device.
                    if (__LoadResources && credentialsOk)
                    {
                        //d.Resources = LoadResources(deviceNode);
                    }
            }
                // Unreachable devices will be grayed out.
                else
                {
                deviceNode.ForeColor = Color.Gray;
            }
        }

            // Expand the tree so that all loaded devices and resources are visible.
            treeDevices.ExpandAll();

            Cursor.Current = Cursors.Default;   // Clear the "busy" cursor
        }
        /// <summary>
        /// Display a form to load the user's Gateway credentials.
        /// </summary>
        /// <returns>a boolean indicating whether or not we successfully set credentials</returns>
        //private bool SetGatewayCoapApiCredentials()
        //{
        //    FileLogger.Write("Fetch Gateway credentials");

        //    // If we have already established credentials don't display the credentials form.
        //    // If credentials are incorrect or we want to use a different set, then they must be reset via the UI.
        //    if (GatewaySettings.Instance.CoApPassword != "" && GatewaySettings.Instance.CoApId != "")
        //        return true;

        //    frmCoApApiLogin f = new frmCoApApiLogin();
        //    DialogResult r = f.ShowDialog();

        //    // OK and CANCEL are the only dialog results allowed.
        //    // If we get an OK, it is assumed that the credentials have been loaded.
        //    if (r == DialogResult.OK)
        //        return true;

        //    FileLogger.Write("Credentials not loaded");

        //    return false;
        //}

        /// <summary>
        /// Load detalis of all CoAP resources associated with a device
        /// </summary>
        /// <param name="device">The tree node in the device list containing the device</param>
        private CoApResources LoadResources(TreeNode device)
        {
            Cursor.Current = Cursors.WaitCursor;    // Show as buys while loading the resources.

            FileLogger.Write("Loading all known resources for device " + device.Text);
            device.Nodes.Clear();

            CoApDiscovery disc = (CoApDiscovery)NetworkInterface.Instance.DiscoveryRequest;
            disc.ServerPort = GatewaySettings.Instance.CoApPort;
            disc.IpAddress = device.Text;

            CoApResources resources = disc.LoadResources();
            // Display the error where we would otherwise have displayed the resources.
            if (disc.ErrorResult != "")
            {
                TreeNode[] resNode = new TreeNode[1];
                device.ForeColor = Color.Red;
                resNode[0] = new TreeNode(disc.ErrorResult);
                device.Nodes.AddRange(resNode);
            }

            if (resources != null && disc.ErrorResult == "")
            {
                 FileLogger.Write("Resources returned");
                foreach (CoApResource res in resources)
                {
                    TreeNode[] resNode = new TreeNode[1];
                    resNode[0] = new TreeNode(res.URI);
                    device.Nodes.AddRange(resNode);

                    foreach (CoApLinkAttribute a in res.LinkAttributes)
                    {
                        TreeNode[] node = new TreeNode[1];
                        node[0] = new TreeNode(a.ToString());
                        resNode[0].Nodes.AddRange(node);
                    }
                }
            }
            else
            {
                FileLogger.Write("Resources NOT returned");
            }
            disc = null;
            GC.Collect();
            Cursor.Current = Cursors.Default;   // Clear the "busy" cursor
            return resources;
        }
        /// <summary>
        /// Double-clicking a resource results in making a CoAP call to fetch the contents.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void treeDevices_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            pnlResults.Visible = false;
            pnlResults.Top = treeDevices.Location.X + e.Location.X;
            pnlResults.Left = treeDevices.Location.Y + e.Location.Y;

            if (e.Node.Parent == null)
            {
                LoadResources(e.Node);
                return;
            }

            GetResource(e.Node, e.Location);
        }
        /// <summary>
        /// Get the resource closest to the clicked location.
        /// </summary>
        /// <param name="node">the node currently selected</param>
        /// <param name="location">the screen location where the results will be displayed</param>
        private void GetResource(TreeNode node, Point location)
        {
            if (node == null)
            {
                return;
            }
            frmGet g = new frmGet();
            g.DeviceName = GetDeviceName(node);
            g.URI = GetResourceName(node);
            if (g.URI == g.DeviceName)
            {
                g.URI = "/";
            }
            g.ShowDialog();
            return;
        }
        /// <summary>
        /// Fetch the device name being queried
        /// </summary>
        /// <param name="clicked"></param>
        /// <returns>the parent device of the resource being queried</returns>
        private string GetDeviceName(TreeNode clicked)
        {
            TreeNode t = clicked;
            while (t.Parent != null)
            {
                t = t.Parent;
            }
            return t.Text;
        }
        /// <summary>
        /// Fetch the name of the resource to be queried
        /// </summary>
        /// <param name="clicked">the node that we have selected for resource retrieval</param>
        /// <returns>the name of the resource to be queried</returns>
        private string GetResourceName(TreeNode clicked)
        {
            TreeNode t = clicked;
            TreeNode prev = clicked;
            while (t.Parent != null)
            {
                prev = t;
                t = t.Parent;
            }
            return prev.Text;
        }
        /// <summary>
        /// Capture the Confirmable/nonconfirmable value
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void radNonConfirmable_CheckedChanged(object sender, EventArgs e)
        {
            CoApSettings.Instance.NonConfirmable = radNonConfirmable.Checked;
        }
        /// <summary>
        /// Toggle use of the Gateway as the communication method
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void radGateway_CheckedChanged(object sender, EventArgs e)
        {
            // Clear any user settings.
            ClearAll();

            // If this radio button is not checked we are just getting the 
            // checked changed event because another one was checked.
            // We can just ignore it.
            if (!radGateway.Checked)
            {
                return;
            }

            // Change the interface setting.
            NetworkInterface.Instance.SelectedInterface = NetworkInterface.NetworkOption.Gateway;

            // We need credentials for this interface method.
            // If we get them then we can proceed to load all devices and resources.
            if (SetGatewayGatewayCredentials())
            {
                LoadNodes();
            }
        }
        /// <summary>
        /// Capture the SLDP API credentials
        /// </summary>
        /// <returns>a boolean indicating whether the credentials were successfully set</returns>
        private bool SetGatewayGatewayCredentials()
        {
            // If we have already established credentials don't display the credentials form.
            // If credentials are incorrect or we want to use a different set, then they must be reset via the UI.
            if (GatewaySettings.Instance.ClientSecret != "" && GatewaySettings.Instance.ClientId != "" && GatewaySettings.Instance.Solution != "")
                return true;

            frmGatewayLogin f = new frmGatewayLogin();
            DialogResult r = f.ShowDialog();

            // OK and CANCEL are the only dialog results allowed.
            // If we get an OK, it is assumed that the credentials have been loaded.
            if (r == DialogResult.OK)
                return true;
            return false;

        }

        private void btnCloseResults_Click(object sender, EventArgs e)
        {
            pnlResults.Visible = false;
        }
        private void mnuFunctions_MouseLeave(object sender, EventArgs e)
        {
            if (mnuFunctions.Visible)
            {
                mnuFunctions.Visible = false;
            }
        }
        /// <summary>
        /// Request CoAP confirmable messaging.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void radConfirmable_CheckedChanged(object sender, EventArgs e)
        {
            CoApSettings.Instance.Confirmable = radConfirmable.Checked;
        }
        /// <summary>
        /// Read the selected device resource.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void mnuResourceGet_Click(object sender, EventArgs e)
        {
            GetResource(treeDevices.SelectedNode, treeDevices.Location);
        }

        private void mnuResource_Click(object sender, EventArgs e)
        {
            //mnuResourceGet.Enabled = false;
            mnuResourceDelete.Enabled = false;
            mnuResourceObserve.Enabled = false;
            mnuResourcePut.Enabled = false;

            if (treeDevices == null)
                return;

            if (treeDevices.Nodes == null)
                return;

            if (treeDevices.SelectedNode == null)
                return;

            if (treeDevices.SelectedNode.Parent == null)
            {
                //mnuResourceGet.Enabled = false;
                mnuResourceDelete.Enabled = false;
                mnuResourceObserve.Enabled = false;
                mnuResourcePut.Enabled = false;
            }
            else
            {
                mnuResourceGet.Enabled = true;
                mnuResourceDelete.Enabled = true;
                mnuResourceObserve.Enabled = true;
                mnuResourcePut.Enabled = true;
            }
        }


        private void treeDevices_BeforeSelect(object sender, TreeViewCancelEventArgs e)
        {
            // maybe remove the unreachable text and just gray out and ignore?
        }

        private void mnuRefreshDevices_Click(object sender, EventArgs e)
        {
            ClearAll();

            if (NetworkInterface.Instance.SelectedInterface == NetworkInterface.NetworkOption.Gateway)

            {

                if (SetGatewayGatewayCredentials())
                {
                    LoadNodes();
                    return;
                }
            }

            LoadNodes();
        }
        /// <summary>
        /// Put to the selected device resource
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void mnuPut_Click(object sender, EventArgs e)
        {
            PutResource(treeDevices.SelectedNode);
        }

        private void PutResource(TreeNode node)
        {
        }

        private void frmNewMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (__Observer != null)
            {
                if (__Observer.Observing)
                {
                    __Observer.TerminateObserving();
                }
            }
            try
            {
                CoApGatewaySessionManager.Instance.ShutDown();
            }
            catch { }
        }

        private void mnuDelete_Click(object sender, EventArgs e)
        {

        }

        private void mnuResourcePut_Click(object sender, EventArgs e)
        {
            frmPut p = new frmPut();
            p.DeviceName = GetDeviceName(treeDevices.SelectedNode);
            p.ResourceName = GetResourceName(treeDevices.SelectedNode);
            p.ShowDialog();
        }

        private void mnuClearCredentials_Click(object sender, EventArgs e)
        {
            CoApGatewaySessionManager.Instance.TerminateSession();
            GatewaySettings.Instance.ClearCredentials();
        }

        private void mnuResourceDelete_Click(object sender, EventArgs e)
        {
            DeleteResource(treeDevices.SelectedNode, treeDevices.Location);
        }
        /// <summary>
        /// Get the resource closest to the clicked location.
        /// </summary>
        /// <param name="node">the node currently selected</param>
        /// <param name="location">the screen location where the results will be displayed</param>
        private void DeleteResource(TreeNode node, Point location)
        {
            Cursor.Current = Cursors.WaitCursor;

            CoApDelete delete = (CoApDelete)NetworkInterface.Instance.ResourceDeleter;

            // Drill into the device tree and fetch the related device name.
            delete.IpAddress = GetDeviceName(node);
            delete.ServerPort = GatewaySettings.Instance.CoApPort;
            // Drill into the device tree and fetch the related resource to fetch.
            delete.URI = GetResourceName(node);
            delete.Send();
            try
            {
                txtResult.Text = delete.Response.ToString();
            }
            catch
            {
                txtResult.Text = "Unable to retreive response from resource";
            }
            // Adjust the results location to approximate the resource location.
            pnlResults.Top = treeDevices.Location.X + location.X;
            pnlResults.Left = treeDevices.Location.Y + location.Y;
            pnlResults.Location = new Point(location.X + (pnlResults.Width / 5), location.Y);

            // Based on the display location, adjust the height of the results panel.
            int bottom = pnlEdge.Bottom;
            pnlResults.Height = Math.Min((bottom - pnlResults.Top), __PreferredResultsHeight);

            // Finally, make the results visible.
            pnlResults.Visible = true;
            Cursor.Current = Cursors.Default;
        }

        private void cboPort_SelectedIndexChanged(object sender, EventArgs e)
        {
            GatewaySettings.Instance.CoApPort = Convert.ToInt32(cboPort.SelectedItem);
        }
        /// <summary>
        /// Handle Timeout changes.
        /// Errors in
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void txtTimeout_TextChanged(object sender, EventArgs e)
        {
            try
            {
                GatewaySettings.Instance.RequestTimeout = Convert.ToInt32(txtTimeout.Text) * 1000;
            }
            catch
            {
                txtTimeout.SelectAll();
                txtTimeout.Focus();
            }
        }

        private void mnuResourceObserve_Click(object sender, EventArgs e)
        {
            frmObserve g = new frmObserve();
            g.DeviceName = GetDeviceName(treeDevices.SelectedNode);
            g.URI = GetResourceName(treeDevices.SelectedNode);
            g.ShowDialog();
            return;
        }
        /// <summary>
        /// Handle the "auto discover" event.
        /// When this radio button is checked, we automatically refresh resources related to a device.
        /// If it is NOT checked, then we don't do the resource refresh, just the device refresh.
        /// </summary>
        /// <param name="sender">the object initiating the request</param>
        /// <param name="e">all related events</param>
        private void radAutoDiscover_CheckedChanged(object sender, EventArgs e)
        {
            __LoadResources = radAutoDiscover.Checked;
        }
        /// <summary>
        /// Handle About menu item click
        /// </summary>
        /// <param name="sender">the object related to the click event</param>
        /// <param name="e">all events related to the request</param>
        private void mnuAbout_Click(object sender, EventArgs e)
        {
            // Fetch the version number from this assembly.
            System.Reflection.Assembly thisAssembly = System.Reflection.Assembly.GetExecutingAssembly();

            MessageBox.Show(this.Text + "\r\nVersion " + HdkUtils.GetAssemblyVersion(thisAssembly)
                + "\r\nCopyright 2016, 2017 Silver Spring Networks", "About HDK CoAP Client", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void radSimulator_CheckedChanged(object sender, EventArgs e)
        {
            // Clear any user settings.
            ClearAll();

            // If this radio button is not checked we are just getting the 
            // checked changed event because another one was checked.
            // We can just ignore it.
            if (!radSimulator.Checked)
            {
                return;
            }

            // Set the interface setting.
            NetworkInterface.Instance.SelectedInterface = NetworkInterface.NetworkOption.Simulator;
            // Default network addressing is IPv6.
            CoAPSettings.Instance.AddressFamily = System.Net.Sockets.AddressFamily.InterNetworkV6;

            // Reload all devices and related resources.
            LoadNodes();
        }


        private void button1_Click_1(object sender, EventArgs e)
        {
            
            Cursor.Current = Cursors.WaitCursor;
            try
            {

                if (CoApGatewaySessionManager.Instance.IsSessionEstablished())
                {
                    // Terminate and re-establish session
                    CoApGatewaySessionManager.Instance.TerminateSession();
                    CoApGatewaySessionManager.Instance.EstablishSession();
                }
                // Check status again.  It not established then Token stale and need new Token
                if (!CoApGatewaySessionManager.Instance.IsSessionEstablished())
                { 
                    mnuRefreshDevices_Click(this, EventArgs.Empty);  // refresh of devices gets new Token
                    CoApGatewaySessionManager.Instance.EstablishSession();
                }
            } catch (Exception ex)
            {
                FileLogger.Write("Problem Establishing Session - " + ex.Message);
            }
            finally
            {
                Cursor.Current = Cursors.Default;
            }
        }

        private void mnuDevice_Click(object sender, EventArgs e)
        {
            mnuRefreshResources.Enabled = false;

            if (treeDevices == null)
                return;

            if (treeDevices.Nodes == null)
                return;

            if (treeDevices.SelectedNode == null)
                return;

            mnuRefreshResources.Enabled = true;

            //if (treeDevices.SelectedNode.Parent == null)
            //{
            //    mnuRefreshResources.Enabled = false;
            //}
            //else
            //{
            //    mnuRefreshResources.Enabled = true;
            //}
        }

        private void mnuRefreshResources_Click(object sender, EventArgs e)
        {
            string deviceName = GetDeviceName(treeDevices.SelectedNode);
            if (deviceName != "")
            {
                TreeNode device = GetDeviceNode(deviceName);
                if (device != null)
                {
                    LoadResources(device);
                }
            }
        }
        private TreeNode GetDeviceNode(string deviceName)
        {
            foreach(TreeNode n in treeDevices.Nodes)
            {
                if(n.Text == deviceName)
                {
                    return n;
                }
            }
            return null;
        }

        private void btnConnectionSettings_Click(object sender, EventArgs e)
        {
            frmConnectionSettings c = new frmConnectionSettings();
            c.ShowDialog();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;
            CoApGatewaySessionManager.Instance.TerminateSession();
            Cursor.Current = Cursors.Default;

        }
    }
}
