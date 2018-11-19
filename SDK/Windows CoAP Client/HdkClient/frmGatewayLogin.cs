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

using System.Windows.Forms;

namespace HdkClient
{
    /// <summary>
    /// Login screen for SLDP API.
    /// </summary>
    public partial class frmGatewayLogin : Form
    {
        private string __ClientId = "";
        private string __ClientSecret = "";
        private bool __CanClose = false;
        /// <summary>
        /// Default constructor
        /// </summary>
        public frmGatewayLogin()
        {
            InitializeComponent();
        }
        /// <summary>
        /// Client Id
        /// </summary>
        public string ClientId
        {
            set
            {
                __ClientId = value;
                txtClientId.Text = __ClientId;
            }
        }
        /// <summary>
        /// Client password
        /// </summary>
        public string ClientSecret
        {
            set
            {
                __ClientSecret = value;
                txtPassword.Text = __ClientSecret;
            }
        }
        /// <summary>
        /// Handle OK button
        /// </summary>
        /// <param name="sender">button object</param>
        /// <param name="e">event fired by the click</param>
        private void btnOk_Click(object sender, EventArgs e)
        {
            // Make sure that the client has been specified.
            if (txtClientId.Text.Trim() == "")
            {
                MessageBox.Show("Clent Id cannot be blank");
                txtClientId.SelectAll();
                txtClientId.Focus();
                __CanClose = false;
                return;
            }

            // Make sure that the password has been specified.
            if (txtPassword.Text.Trim() == "")
            {
                MessageBox.Show("Clent Secret cannot be blank");
                txtPassword.SelectAll();
                txtPassword.Focus();
                __CanClose = false;
                return;
            }

            // Make sure that the customer id has been specified.
            if (txtSolution.Text.Trim() == "")
            {
                MessageBox.Show("Solution cannot be blank");
                txtSolution.SelectAll();
                txtSolution.Focus();
                __CanClose = false;
                return;
            }

            // persist ClientId, ClientSecret, and Solution
            Properties.Settings.Default.ClientId = txtClientId.Text;
            Properties.Settings.Default.ClientSecret = txtPassword.Text;
            Properties.Settings.Default.Solution = txtSolution.Text;


            // If all required fields have been specified we can close the form.
            __CanClose = true;
        }
        /// <summary>
        /// See if we can close the form.
        /// If we clicked cancel, we can close.
        /// If we clicked OK but the fields aren't correct, we CAN'T close.
        /// </summary>
        /// <param name="sender">the object associated with the close operation</param>
        /// <param name="e">any related close event arguments</param>
        private void frmSdpLogin_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Decide whether it is ok to close the form.
            e.Cancel = !__CanClose;
            // Set the static credentials prior to closeing the form.
            GatewaySettings.Instance.ClientId = txtClientId.Text.Trim();
            GatewaySettings.Instance.ClientSecret = txtPassword.Text.Trim();
            GatewaySettings.Instance.Solution = txtSolution.Text.Trim();
        }
        /// <summary>
        /// Handle cancel request
        /// </summary>
        /// <param name="sender">the object associated with the cancel request</param>
        /// <param name="e">any related cancel event arguments</param>
        private void btnCancel_Click(object sender, EventArgs e)
        {
            // Allow the form to close.
            __CanClose = true;
            // The caller will see a cancel response from ShowDialog()
            this.DialogResult = DialogResult.Cancel;
        }

        private void frmGatewayLogin_Load(object sender, EventArgs e)
        {
            txtClientId.Text = Properties.Settings.Default.ClientId;
            txtPassword.Text = Properties.Settings.Default.ClientSecret;
            txtSolution.Text = Properties.Settings.Default.Solution;

        }
    }
}
