namespace HdkClient
{
    partial class frmConnectionSettings
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOk = new System.Windows.Forms.Button();
            this.txtDevicePrefix = new System.Windows.Forms.TextBox();
            this.lblDevicePrefix = new System.Windows.Forms.Label();
            this.lblDeviceSuffix = new System.Windows.Forms.Label();
            this.txtDeviceSuffix = new System.Windows.Forms.TextBox();
            this.lblGwURI = new System.Windows.Forms.Label();
            this.txtGatewayUri = new System.Windows.Forms.TextBox();
            this.lblGwBaseUrl = new System.Windows.Forms.Label();
            this.txtGatewayUrl = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(389, 227);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 9;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnOk
            // 
            this.btnOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOk.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOk.Location = new System.Drawing.Point(389, 198);
            this.btnOk.Name = "btnOk";
            this.btnOk.Size = new System.Drawing.Size(75, 23);
            this.btnOk.TabIndex = 8;
            this.btnOk.Text = "OK";
            this.btnOk.UseVisualStyleBackColor = true;
            this.btnOk.Click += new System.EventHandler(this.btnOk_Click);
            // 
            // txtDevicePrefix
            // 
            this.txtDevicePrefix.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtDevicePrefix.Location = new System.Drawing.Point(170, 57);
            this.txtDevicePrefix.Name = "txtDevicePrefix";
            this.txtDevicePrefix.Size = new System.Drawing.Size(201, 20);
            this.txtDevicePrefix.TabIndex = 1;
            // 
            // lblDevicePrefix
            // 
            this.lblDevicePrefix.AutoSize = true;
            this.lblDevicePrefix.Location = new System.Drawing.Point(37, 57);
            this.lblDevicePrefix.Name = "lblDevicePrefix";
            this.lblDevicePrefix.Size = new System.Drawing.Size(70, 13);
            this.lblDevicePrefix.TabIndex = 0;
            this.lblDevicePrefix.Text = "Device &Prefix";
            // 
            // lblDeviceSuffix
            // 
            this.lblDeviceSuffix.AutoSize = true;
            this.lblDeviceSuffix.Location = new System.Drawing.Point(37, 83);
            this.lblDeviceSuffix.Name = "lblDeviceSuffix";
            this.lblDeviceSuffix.Size = new System.Drawing.Size(70, 13);
            this.lblDeviceSuffix.TabIndex = 2;
            this.lblDeviceSuffix.Text = "Device &Suffix";
            // 
            // txtDeviceSuffix
            // 
            this.txtDeviceSuffix.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtDeviceSuffix.Location = new System.Drawing.Point(170, 83);
            this.txtDeviceSuffix.Name = "txtDeviceSuffix";
            this.txtDeviceSuffix.Size = new System.Drawing.Size(201, 20);
            this.txtDeviceSuffix.TabIndex = 3;
            // 
            // lblGwURI
            // 
            this.lblGwURI.AutoSize = true;
            this.lblGwURI.Location = new System.Drawing.Point(37, 109);
            this.lblGwURI.Name = "lblGwURI";
            this.lblGwURI.Size = new System.Drawing.Size(71, 13);
            this.lblGwURI.TabIndex = 4;
            this.lblGwURI.Text = "Gateway UR&I";
            // 
            // txtGatewayUri
            // 
            this.txtGatewayUri.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtGatewayUri.Location = new System.Drawing.Point(170, 109);
            this.txtGatewayUri.Name = "txtGatewayUri";
            this.txtGatewayUri.Size = new System.Drawing.Size(201, 20);
            this.txtGatewayUri.TabIndex = 5;
            // 
            // lblGwBaseUrl
            // 
            this.lblGwBaseUrl.AutoSize = true;
            this.lblGwBaseUrl.Location = new System.Drawing.Point(37, 135);
            this.lblGwBaseUrl.Name = "lblGwBaseUrl";
            this.lblGwBaseUrl.Size = new System.Drawing.Size(101, 13);
            this.lblGwBaseUrl.TabIndex = 6;
            this.lblGwBaseUrl.Text = "Gateway Base UR&L";
            // 
            // txtGatewayUrl
            // 
            this.txtGatewayUrl.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtGatewayUrl.Location = new System.Drawing.Point(170, 135);
            this.txtGatewayUrl.Name = "txtGatewayUrl";
            this.txtGatewayUrl.Size = new System.Drawing.Size(201, 20);
            this.txtGatewayUrl.TabIndex = 7;
            // 
            // frmConnectionSettings
            // 
            this.AcceptButton = this.btnOk;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(476, 262);
            this.Controls.Add(this.lblGwBaseUrl);
            this.Controls.Add(this.txtGatewayUrl);
            this.Controls.Add(this.lblGwURI);
            this.Controls.Add(this.txtGatewayUri);
            this.Controls.Add(this.lblDeviceSuffix);
            this.Controls.Add(this.txtDeviceSuffix);
            this.Controls.Add(this.lblDevicePrefix);
            this.Controls.Add(this.txtDevicePrefix);
            this.Controls.Add(this.btnOk);
            this.Controls.Add(this.btnCancel);
            this.MinimumSize = new System.Drawing.Size(492, 301);
            this.Name = "frmConnectionSettings";
            this.Text = "Connection Settings";
            this.Load += new System.EventHandler(this.frmConnectionSettings_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOk;
        private System.Windows.Forms.TextBox txtDevicePrefix;
        private System.Windows.Forms.Label lblDevicePrefix;
        private System.Windows.Forms.Label lblDeviceSuffix;
        private System.Windows.Forms.TextBox txtDeviceSuffix;
        private System.Windows.Forms.Label lblGwURI;
        private System.Windows.Forms.TextBox txtGatewayUri;
        private System.Windows.Forms.Label lblGwBaseUrl;
        private System.Windows.Forms.TextBox txtGatewayUrl;
    }
}