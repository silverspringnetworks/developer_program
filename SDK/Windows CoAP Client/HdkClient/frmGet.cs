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
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace HdkClient
{
    /// <summary>
    /// Form for managing GET requests.
    /// </summary>
    public partial class frmGet : Form
    {
        private string __DeviceName = "";
        private string __URI = "";
        /// <summary>
        /// The name of the target device
        /// </summary>
        public string DeviceName
        {
            get { return __DeviceName; }
            set { __DeviceName = value; }
        }
        /// <summary>
        /// The target resource in URI format.
        /// </summary>
        public string URI
        {
            get { return __URI; }
            set { __URI = value; }
        }
        /// <summary>
        /// Default constructor
        /// </summary>
        public frmGet()
        {
            InitializeComponent();
        }
        /// <summary>
        /// Initial form load handler
        /// </summary>
        /// <param name="sender">the object generating the load event</param>
        /// <param name="e">all load-related events</param>
        private void frmGet_Load(object sender, EventArgs e)
        {
            // The device name and the URI for the request should have been preloaded by the caller.
            // Set these values on the UI.
            txtDevice.Text = __DeviceName;
            txtURI.Text = __URI;
        }
        /// <summary>
        /// Handle GET request
        /// </summary>
        /// <param name="sender">the object generating the click</param>
        /// <param name="e">click-related arguments</param>
        private void btnGet_Click(object sender, EventArgs e)
        {
            // Set the wait cursor.
            Cursor.Current = Cursors.WaitCursor;

            // Hide any previous results
            pnlResults.Visible = false;
            lblResult.Visible = false;

            CoApGet get = (CoApGet)NetworkInterface.Instance.ResourceGetter;

            // Use the device property for the target location.
            get.IpAddress = txtDevice.Text;

            // Use the pre-defined CoAP port number.
            get.ServerPort = GatewaySettings.Instance.CoApPort;

            // Use the resource property for the target resource.
            get.URI = txtURI.Text;

            // Send the request!
            get.Send();

            // Parse out the GET response.
            try
            {
                txtResult.Text = get.Response.ToString();
                if (get.Response.Payload.Value != null)
                {
                    if (get.Response.Payload.Value.Length != 0)
                    {
                        txtResult.Text += HdkUtils.BytesToHexView(get.Response.Payload.Value);
                    }
                }
            }
            catch
            {
                txtResult.Text = "Unable to retreive response from resource";
            }

            // Finally, make the results visible.
            pnlResults.Visible = true;
            lblResult.Visible = true;

            // Reset the wait cursor.
            Cursor.Current = Cursors.Default;
        }
    }
}
