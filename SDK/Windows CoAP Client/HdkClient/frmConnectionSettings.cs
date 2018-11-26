using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace HdkClient
{
    public partial class frmConnectionSettings : Form
    {
        public frmConnectionSettings()
        {
            InitializeComponent();
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            if (ValidSettings())
            {
                GatewaySettings.Instance.GatewayDeviceURIPrefix = txtDevicePrefix.Text.Trim();
                GatewaySettings.Instance.GatewayDeviceURISuffix = txtDeviceSuffix.Text.Trim();
                GatewaySettings.Instance.GatewayURI = txtGatewayUri.Text.Trim();
                GatewaySettings.Instance.GatewayBaseURL = txtGatewayUrl.Text.Trim();
                GatewaySettings.Instance.Persist();
                this.Close();
            }
        }

        private void frmConnectionSettings_Load(object sender, EventArgs e)
        {
            txtDevicePrefix.Text = GatewaySettings.Instance.GatewayDeviceURIPrefix;
            txtDeviceSuffix.Text = GatewaySettings.Instance.GatewayDeviceURISuffix;
            txtGatewayUri.Text = GatewaySettings.Instance.GatewayURI;
            txtGatewayUrl.Text = GatewaySettings.Instance.GatewayBaseURL;
        }
        private bool ValidSettings()
        {
            return true;
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            GatewaySettings.Instance.Restore();
        }
    }
}
