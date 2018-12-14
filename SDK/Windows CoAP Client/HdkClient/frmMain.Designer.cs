namespace HdkClient
{
    partial class frmMain
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmMain));
            this.btnRun = new System.Windows.Forms.Button();
            this.grpNetworkInterface = new System.Windows.Forms.GroupBox();
            this.radLAN = new System.Windows.Forms.RadioButton();
            this.radFSU = new System.Windows.Forms.RadioButton();
            this.radGateway = new System.Windows.Forms.RadioButton();
            this.lstDevices = new System.Windows.Forms.ListBox();
            this.btnRefresh = new System.Windows.Forms.Button();
            this.lblDeviceList = new System.Windows.Forms.Label();
            this.lblPort = new System.Windows.Forms.Label();
            this.cboPort = new System.Windows.Forms.ComboBox();
            this.ctrlTip = new System.Windows.Forms.ToolTip(this.components);
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.btnImportIpAddresses = new System.Windows.Forms.ToolStripButton();
            this.treeResources = new System.Windows.Forms.TreeView();
            this.txtResult = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.grpResponseModel = new System.Windows.Forms.GroupBox();
            this.radNonConfirmable = new System.Windows.Forms.RadioButton();
            this.radConfirmable = new System.Windows.Forms.RadioButton();
            this.txtTimeout = new System.Windows.Forms.TextBox();
            this.lblTimeOut = new System.Windows.Forms.Label();
            this.btnDebug = new System.Windows.Forms.Button();
            this.grpNetworkInterface.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            this.grpResponseModel.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnRun
            // 
            this.btnRun.Image = ((System.Drawing.Image)(resources.GetObject("btnRun.Image")));
            this.btnRun.Location = new System.Drawing.Point(452, 60);
            this.btnRun.Name = "btnRun";
            this.btnRun.Size = new System.Drawing.Size(28, 30);
            this.btnRun.TabIndex = 0;
            this.btnRun.UseVisualStyleBackColor = true;
            this.btnRun.Click += new System.EventHandler(this.btnRun_Click);
            this.btnRun.MouseHover += new System.EventHandler(this.btnRun_MouseHover);
            // 
            // grpNetworkInterface
            // 
            this.grpNetworkInterface.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.grpNetworkInterface.Controls.Add(this.radLAN);
            this.grpNetworkInterface.Controls.Add(this.radFSU);
            this.grpNetworkInterface.Controls.Add(this.radGateway);
            this.grpNetworkInterface.Location = new System.Drawing.Point(1062, 45);
            this.grpNetworkInterface.Name = "grpNetworkInterface";
            this.grpNetworkInterface.Size = new System.Drawing.Size(119, 100);
            this.grpNetworkInterface.TabIndex = 1;
            this.grpNetworkInterface.TabStop = false;
            this.grpNetworkInterface.Text = "Network Interface";
            // 
            // radLAN
            // 
            this.radLAN.AutoSize = true;
            this.radLAN.Checked = true;
            this.radLAN.Location = new System.Drawing.Point(22, 28);
            this.radLAN.Name = "radLAN";
            this.radLAN.Size = new System.Drawing.Size(46, 17);
            this.radLAN.TabIndex = 2;
            this.radLAN.TabStop = true;
            this.radLAN.Text = "LAN";
            this.radLAN.UseVisualStyleBackColor = true;
            this.radLAN.CheckedChanged += new System.EventHandler(this.radLAN_CheckedChanged);
            // 
            // radFSU
            // 
            this.radFSU.AutoSize = true;
            this.radFSU.Location = new System.Drawing.Point(22, 74);
            this.radFSU.Name = "radFSU";
            this.radFSU.Size = new System.Drawing.Size(46, 17);
            this.radFSU.TabIndex = 1;
            this.radFSU.Text = "FSU";
            this.radFSU.UseVisualStyleBackColor = true;
            this.radFSU.CheckedChanged += new System.EventHandler(this.radFSU_CheckedChanged);
            // 
            // radGateway
            // 
            this.radGateway.AutoSize = true;
            this.radGateway.Location = new System.Drawing.Point(22, 51);
            this.radGateway.Name = "radGateway";
            this.radGateway.Size = new System.Drawing.Size(97, 17);
            this.radGateway.TabIndex = 0;
            this.radGateway.Text = "CoAP Gateway";
            this.radGateway.UseVisualStyleBackColor = true;
            this.radGateway.CheckedChanged += new System.EventHandler(this.radGateway_CheckedChanged);
            // 
            // lstDevices
            // 
            this.lstDevices.AllowDrop = true;
            this.lstDevices.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.lstDevices.FormattingEnabled = true;
            this.lstDevices.Location = new System.Drawing.Point(12, 60);
            this.lstDevices.Name = "lstDevices";
            this.lstDevices.Size = new System.Drawing.Size(229, 381);
            this.lstDevices.TabIndex = 2;
            this.lstDevices.Click += new System.EventHandler(this.lstDevices_Click);
            this.lstDevices.DoubleClick += new System.EventHandler(this.lstDevices_DoubleClick);
            this.lstDevices.MouseHover += new System.EventHandler(this.lstDevices_MouseHover);
            // 
            // btnRefresh
            // 
            this.btnRefresh.AutoSize = true;
            this.btnRefresh.Image = ((System.Drawing.Image)(resources.GetObject("btnRefresh.Image")));
            this.btnRefresh.Location = new System.Drawing.Point(125, 25);
            this.btnRefresh.Name = "btnRefresh";
            this.btnRefresh.Size = new System.Drawing.Size(30, 30);
            this.btnRefresh.TabIndex = 3;
            this.btnRefresh.UseVisualStyleBackColor = true;
            this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
            this.btnRefresh.MouseHover += new System.EventHandler(this.btnRefresh_MouseHover);
            // 
            // lblDeviceList
            // 
            this.lblDeviceList.AutoSize = true;
            this.lblDeviceList.Location = new System.Drawing.Point(16, 12);
            this.lblDeviceList.Name = "lblDeviceList";
            this.lblDeviceList.Size = new System.Drawing.Size(103, 13);
            this.lblDeviceList.TabIndex = 4;
            this.lblDeviceList.Text = "Discovered Devices";
            // 
            // lblPort
            // 
            this.lblPort.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lblPort.AutoSize = true;
            this.lblPort.Location = new System.Drawing.Point(1061, 148);
            this.lblPort.Name = "lblPort";
            this.lblPort.Size = new System.Drawing.Size(26, 13);
            this.lblPort.TabIndex = 5;
            this.lblPort.Text = "Port";
            // 
            // cboPort
            // 
            this.cboPort.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cboPort.FormattingEnabled = true;
            this.cboPort.Items.AddRange(new object[] {
            "5683",
            "5684"});
            this.cboPort.Location = new System.Drawing.Point(1062, 164);
            this.cboPort.Name = "cboPort";
            this.cboPort.Size = new System.Drawing.Size(83, 21);
            this.cboPort.TabIndex = 6;
            // 
            // toolStrip1
            // 
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnImportIpAddresses});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(1193, 25);
            this.toolStrip1.TabIndex = 8;
            this.toolStrip1.Text = "toolStrip1";
            // 
            // btnImportIpAddresses
            // 
            this.btnImportIpAddresses.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.btnImportIpAddresses.Image = ((System.Drawing.Image)(resources.GetObject("btnImportIpAddresses.Image")));
            this.btnImportIpAddresses.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnImportIpAddresses.Name = "btnImportIpAddresses";
            this.btnImportIpAddresses.Size = new System.Drawing.Size(23, 22);
            this.btnImportIpAddresses.Text = "Load IP Address List";
            this.btnImportIpAddresses.Click += new System.EventHandler(this.btnImportIpAddresses_Click);
            // 
            // treeResources
            // 
            this.treeResources.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.treeResources.Location = new System.Drawing.Point(247, 60);
            this.treeResources.Name = "treeResources";
            this.treeResources.Size = new System.Drawing.Size(199, 238);
            this.treeResources.TabIndex = 9;
            this.treeResources.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeResources_NodeMouseDoubleClick);
            this.treeResources.MouseHover += new System.EventHandler(this.treeResources_MouseHover);
            // 
            // txtResult
            // 
            this.txtResult.Location = new System.Drawing.Point(486, 60);
            this.txtResult.Multiline = true;
            this.txtResult.Name = "txtResult";
            this.txtResult.ReadOnly = true;
            this.txtResult.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtResult.Size = new System.Drawing.Size(372, 388);
            this.txtResult.TabIndex = 10;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(864, 164);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(52, 23);
            this.button1.TabIndex = 11;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // treeView1
            // 
            this.treeView1.Location = new System.Drawing.Point(864, 191);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(177, 263);
            this.treeView1.TabIndex = 12;
            // 
            // grpResponseModel
            // 
            this.grpResponseModel.Controls.Add(this.radNonConfirmable);
            this.grpResponseModel.Controls.Add(this.radConfirmable);
            this.grpResponseModel.Location = new System.Drawing.Point(1062, 208);
            this.grpResponseModel.Name = "grpResponseModel";
            this.grpResponseModel.Size = new System.Drawing.Size(119, 68);
            this.grpResponseModel.TabIndex = 13;
            this.grpResponseModel.TabStop = false;
            this.grpResponseModel.Text = "Response Model";
            // 
            // radNonConfirmable
            // 
            this.radNonConfirmable.AutoSize = true;
            this.radNonConfirmable.Location = new System.Drawing.Point(22, 42);
            this.radNonConfirmable.Name = "radNonConfirmable";
            this.radNonConfirmable.Size = new System.Drawing.Size(102, 17);
            this.radNonConfirmable.TabIndex = 3;
            this.radNonConfirmable.Text = "Non-confirmable";
            this.radNonConfirmable.UseVisualStyleBackColor = true;
            // 
            // radConfirmable
            // 
            this.radConfirmable.AutoSize = true;
            this.radConfirmable.Location = new System.Drawing.Point(22, 19);
            this.radConfirmable.Name = "radConfirmable";
            this.radConfirmable.Size = new System.Drawing.Size(80, 17);
            this.radConfirmable.TabIndex = 2;
            this.radConfirmable.Text = "Confirmable";
            this.radConfirmable.UseVisualStyleBackColor = true;
            // 
            // txtTimeout
            // 
            this.txtTimeout.Location = new System.Drawing.Point(1143, 289);
            this.txtTimeout.Name = "txtTimeout";
            this.txtTimeout.Size = new System.Drawing.Size(38, 20);
            this.txtTimeout.TabIndex = 14;
            this.txtTimeout.Text = "5";
            this.txtTimeout.TextChanged += new System.EventHandler(this.txtTimeout_TextChanged);
            // 
            // lblTimeOut
            // 
            this.lblTimeOut.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lblTimeOut.AutoSize = true;
            this.lblTimeOut.Location = new System.Drawing.Point(1061, 292);
            this.lblTimeOut.Name = "lblTimeOut";
            this.lblTimeOut.Size = new System.Drawing.Size(81, 13);
            this.lblTimeOut.TabIndex = 15;
            this.lblTimeOut.Text = "Time Out (secs)";
            // 
            // btnDebug
            // 
            this.btnDebug.Location = new System.Drawing.Point(920, 29);
            this.btnDebug.Name = "btnDebug";
            this.btnDebug.Size = new System.Drawing.Size(75, 23);
            this.btnDebug.TabIndex = 16;
            this.btnDebug.Text = "Debug";
            this.btnDebug.UseVisualStyleBackColor = true;
            this.btnDebug.Click += new System.EventHandler(this.btnDebug_Click);
            // 
            // frmMain
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1193, 466);
            this.Controls.Add(this.btnDebug);
            this.Controls.Add(this.lblTimeOut);
            this.Controls.Add(this.txtTimeout);
            this.Controls.Add(this.grpResponseModel);
            this.Controls.Add(this.treeView1);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.txtResult);
            this.Controls.Add(this.treeResources);
            this.Controls.Add(this.toolStrip1);
            this.Controls.Add(this.cboPort);
            this.Controls.Add(this.lblPort);
            this.Controls.Add(this.lblDeviceList);
            this.Controls.Add(this.btnRefresh);
            this.Controls.Add(this.lstDevices);
            this.Controls.Add(this.grpNetworkInterface);
            this.Controls.Add(this.btnRun);
            this.MinimumSize = new System.Drawing.Size(794, 469);
            this.Name = "frmMain";
            this.Text = "Silver Spring Networks Starfish HDK Client";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmMain_FormClosing);
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.Shown += new System.EventHandler(this.frmMain_Shown);
            this.Resize += new System.EventHandler(this.frmMain_Resize);
            this.grpNetworkInterface.ResumeLayout(false);
            this.grpNetworkInterface.PerformLayout();
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            this.grpResponseModel.ResumeLayout(false);
            this.grpResponseModel.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnRun;
        private System.Windows.Forms.GroupBox grpNetworkInterface;
        private System.Windows.Forms.RadioButton radFSU;
        private System.Windows.Forms.RadioButton radGateway;
        private System.Windows.Forms.ListBox lstDevices;
        private System.Windows.Forms.Button btnRefresh;
        private System.Windows.Forms.Label lblDeviceList;
        private System.Windows.Forms.Label lblPort;
        private System.Windows.Forms.ComboBox cboPort;
        private System.Windows.Forms.ToolTip ctrlTip;
        private System.Windows.Forms.RadioButton radLAN;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton btnImportIpAddresses;
        private System.Windows.Forms.TreeView treeResources;
        private System.Windows.Forms.TextBox txtResult;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.GroupBox grpResponseModel;
        private System.Windows.Forms.RadioButton radNonConfirmable;
        private System.Windows.Forms.RadioButton radConfirmable;
        private System.Windows.Forms.TextBox txtTimeout;
        private System.Windows.Forms.Label lblTimeOut;
        private System.Windows.Forms.Button btnDebug;
    }
}

