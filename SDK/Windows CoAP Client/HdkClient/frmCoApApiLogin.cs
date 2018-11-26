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
    public partial class frmCoApApiLogin : Form
    {
        private string __ClientId = "";
        private string __ClientSecret = "";
        private bool __CanClose = false;
        public frmCoApApiLogin()
        {
            InitializeComponent();
        }

        public string ClientId
        {
            set
            {
                __ClientId = value;
                txtUserId.Text = __ClientId;
            }
        }

        public string ClientSecret
        {
            set
            {
                __ClientSecret = value;
                txtPassword.Text = __ClientSecret;
            }
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            if (txtUserId.Text.Trim() == "")
            {
                MessageBox.Show("Clent Id cannot be blank");
                txtUserId.SelectAll();
                txtUserId.Focus();
                __CanClose = false;
                return;
            }

            if (txtPassword.Text.Trim() == "")
            {
                MessageBox.Show("Password cannot be blank");
                txtPassword.SelectAll();
                txtPassword.Focus();
                __CanClose = false;
                return;
            }

            __CanClose = true;
        }
        private void frmSdpLogin_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = !__CanClose;
            GatewaySettings.Instance.CoApId = txtUserId.Text.Trim();
            GatewaySettings.Instance.CoApPassword = txtPassword.Text.Trim();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            __CanClose = true;
            this.DialogResult = DialogResult.Cancel;
        }
    }
}
