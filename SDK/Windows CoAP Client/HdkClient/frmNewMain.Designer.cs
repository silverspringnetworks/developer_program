namespace HdkClient
{
    partial class frmNewMain
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmNewMain));
            this.treeDevices = new System.Windows.Forms.TreeView();
            this.pnlOptions = new System.Windows.Forms.Panel();
            this.btnConnectionSettings = new System.Windows.Forms.Button();
            this.grpDiscoverModel = new System.Windows.Forms.GroupBox();
            this.radDemandDiscover = new System.Windows.Forms.RadioButton();
            this.radAutoDiscover = new System.Windows.Forms.RadioButton();
            this.btnCloseOptions = new System.Windows.Forms.Button();
            this.grpNetworkInterface = new System.Windows.Forms.GroupBox();
            this.radSimulator = new System.Windows.Forms.RadioButton();
            this.radGateway = new System.Windows.Forms.RadioButton();
            this.grpResponseModel = new System.Windows.Forms.GroupBox();
            this.radNonConfirmable = new System.Windows.Forms.RadioButton();
            this.radConfirmable = new System.Windows.Forms.RadioButton();
            this.lblPort = new System.Windows.Forms.Label();
            this.cboPort = new System.Windows.Forms.ComboBox();
            this.lblTimeOut = new System.Windows.Forms.Label();
            this.txtTimeout = new System.Windows.Forms.TextBox();
            this.pnlEdge = new System.Windows.Forms.Panel();
            this.mnuFunctions = new System.Windows.Forms.MenuStrip();
            this.mnuOptions = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuDebug = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuRefreshDevices = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuDevice = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuRefreshResources = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuResource = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuResourceGet = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuResourcePut = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuResourceObserve = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuResourceDelete = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuClearCredentials = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuAbout = new System.Windows.Forms.ToolStripMenuItem();
            this.pnlResults = new System.Windows.Forms.Panel();
            this.btnCloseResults = new System.Windows.Forms.Button();
            this.txtResult = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.txtLog = new System.Windows.Forms.TextBox();
            this.button2 = new System.Windows.Forms.Button();
            this.pnlOptions.SuspendLayout();
            this.grpDiscoverModel.SuspendLayout();
            this.grpNetworkInterface.SuspendLayout();
            this.grpResponseModel.SuspendLayout();
            this.mnuFunctions.SuspendLayout();
            this.pnlResults.SuspendLayout();
            this.SuspendLayout();
            // 
            // treeDevices
            // 
            this.treeDevices.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.treeDevices.Location = new System.Drawing.Point(22, 25);
            this.treeDevices.Name = "treeDevices";
            this.treeDevices.Size = new System.Drawing.Size(650, 410);
            this.treeDevices.TabIndex = 10;
            this.treeDevices.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.treeDevices_BeforeSelect);
            this.treeDevices.NodeMouseDoubleClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.treeDevices_NodeMouseDoubleClick);
            this.treeDevices.MouseDown += new System.Windows.Forms.MouseEventHandler(this.treeDevices_MouseDown);
            // 
            // pnlOptions
            // 
            this.pnlOptions.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlOptions.Controls.Add(this.button2);
            this.pnlOptions.Controls.Add(this.btnConnectionSettings);
            this.pnlOptions.Controls.Add(this.grpDiscoverModel);
            this.pnlOptions.Controls.Add(this.button1);
            this.pnlOptions.Controls.Add(this.btnCloseOptions);
            this.pnlOptions.Controls.Add(this.grpNetworkInterface);
            this.pnlOptions.Controls.Add(this.grpResponseModel);
            this.pnlOptions.Controls.Add(this.lblPort);
            this.pnlOptions.Controls.Add(this.cboPort);
            this.pnlOptions.Controls.Add(this.lblTimeOut);
            this.pnlOptions.Controls.Add(this.txtTimeout);
            this.pnlOptions.Location = new System.Drawing.Point(678, 25);
            this.pnlOptions.Name = "pnlOptions";
            this.pnlOptions.Size = new System.Drawing.Size(157, 410);
            this.pnlOptions.TabIndex = 20;
            // 
            // btnConnectionSettings
            // 
            this.btnConnectionSettings.Location = new System.Drawing.Point(22, 324);
            this.btnConnectionSettings.Name = "btnConnectionSettings";
            this.btnConnectionSettings.Size = new System.Drawing.Size(116, 23);
            this.btnConnectionSettings.TabIndex = 13;
            this.btnConnectionSettings.Text = "Connection Settings";
            this.btnConnectionSettings.UseVisualStyleBackColor = true;
            this.btnConnectionSettings.Click += new System.EventHandler(this.btnConnectionSettings_Click);
            // 
            // grpDiscoverModel
            // 
            this.grpDiscoverModel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.grpDiscoverModel.Controls.Add(this.radDemandDiscover);
            this.grpDiscoverModel.Controls.Add(this.radAutoDiscover);
            this.grpDiscoverModel.Location = new System.Drawing.Point(23, 244);
            this.grpDiscoverModel.Name = "grpDiscoverModel";
            this.grpDiscoverModel.Size = new System.Drawing.Size(119, 68);
            this.grpDiscoverModel.TabIndex = 10;
            this.grpDiscoverModel.TabStop = false;
            this.grpDiscoverModel.Text = "Discovery Model";
            // 
            // radDemandDiscover
            // 
            this.radDemandDiscover.AutoSize = true;
            this.radDemandDiscover.Location = new System.Drawing.Point(17, 42);
            this.radDemandDiscover.Name = "radDemandDiscover";
            this.radDemandDiscover.Size = new System.Drawing.Size(80, 17);
            this.radDemandDiscover.TabIndex = 12;
            this.radDemandDiscover.Text = "On-demand";
            this.radDemandDiscover.UseVisualStyleBackColor = true;
            // 
            // radAutoDiscover
            // 
            this.radAutoDiscover.AutoSize = true;
            this.radAutoDiscover.Location = new System.Drawing.Point(17, 19);
            this.radAutoDiscover.Name = "radAutoDiscover";
            this.radAutoDiscover.Size = new System.Drawing.Size(72, 17);
            this.radAutoDiscover.TabIndex = 11;
            this.radAutoDiscover.Text = "Automatic";
            this.radAutoDiscover.UseVisualStyleBackColor = true;
            this.radAutoDiscover.CheckedChanged += new System.EventHandler(this.radAutoDiscover_CheckedChanged);
            // 
            // btnCloseOptions
            // 
            this.btnCloseOptions.FlatAppearance.BorderSize = 0;
            this.btnCloseOptions.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.btnCloseOptions.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnCloseOptions.Image = global::HdkClient.Properties.Resources.Close_16x;
            this.btnCloseOptions.Location = new System.Drawing.Point(135, 0);
            this.btnCloseOptions.Name = "btnCloseOptions";
            this.btnCloseOptions.Size = new System.Drawing.Size(21, 20);
            this.btnCloseOptions.TabIndex = 26;
            this.btnCloseOptions.UseVisualStyleBackColor = true;
            this.btnCloseOptions.Click += new System.EventHandler(this.btnCloseOptions_Click);
            // 
            // grpNetworkInterface
            // 
            this.grpNetworkInterface.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.grpNetworkInterface.Controls.Add(this.radSimulator);
            this.grpNetworkInterface.Controls.Add(this.radGateway);
            this.grpNetworkInterface.Location = new System.Drawing.Point(23, 14);
            this.grpNetworkInterface.Name = "grpNetworkInterface";
            this.grpNetworkInterface.Size = new System.Drawing.Size(119, 63);
            this.grpNetworkInterface.TabIndex = 0;
            this.grpNetworkInterface.TabStop = false;
            this.grpNetworkInterface.Text = "Network Interface";
            // 
            // radSimulator
            // 
            this.radSimulator.AutoSize = true;
            this.radSimulator.Enabled = false;
            this.radSimulator.Location = new System.Drawing.Point(11, 42);
            this.radSimulator.Name = "radSimulator";
            this.radSimulator.Size = new System.Drawing.Size(68, 17);
            this.radSimulator.TabIndex = 2;
            this.radSimulator.Text = "Simulator";
            this.radSimulator.UseVisualStyleBackColor = true;
            this.radSimulator.Visible = false;
            this.radSimulator.CheckedChanged += new System.EventHandler(this.radSimulator_CheckedChanged);
            // 
            // radGateway
            // 
            this.radGateway.AutoSize = true;
            this.radGateway.Location = new System.Drawing.Point(11, 19);
            this.radGateway.Name = "radGateway";
            this.radGateway.Size = new System.Drawing.Size(97, 17);
            this.radGateway.TabIndex = 1;
            this.radGateway.Text = "CoAP Gateway";
            this.radGateway.UseVisualStyleBackColor = true;
            this.radGateway.CheckedChanged += new System.EventHandler(this.radGateway_CheckedChanged);
            // 
            // grpResponseModel
            // 
            this.grpResponseModel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.grpResponseModel.Controls.Add(this.radNonConfirmable);
            this.grpResponseModel.Controls.Add(this.radConfirmable);
            this.grpResponseModel.Location = new System.Drawing.Point(21, 170);
            this.grpResponseModel.Name = "grpResponseModel";
            this.grpResponseModel.Size = new System.Drawing.Size(119, 68);
            this.grpResponseModel.TabIndex = 7;
            this.grpResponseModel.TabStop = false;
            this.grpResponseModel.Text = "Response Model";
            // 
            // radNonConfirmable
            // 
            this.radNonConfirmable.AutoSize = true;
            this.radNonConfirmable.Location = new System.Drawing.Point(17, 42);
            this.radNonConfirmable.Name = "radNonConfirmable";
            this.radNonConfirmable.Size = new System.Drawing.Size(102, 17);
            this.radNonConfirmable.TabIndex = 9;
            this.radNonConfirmable.Text = "Non-confirmable";
            this.radNonConfirmable.UseVisualStyleBackColor = true;
            this.radNonConfirmable.CheckedChanged += new System.EventHandler(this.radNonConfirmable_CheckedChanged);
            // 
            // radConfirmable
            // 
            this.radConfirmable.AutoSize = true;
            this.radConfirmable.Location = new System.Drawing.Point(17, 19);
            this.radConfirmable.Name = "radConfirmable";
            this.radConfirmable.Size = new System.Drawing.Size(80, 17);
            this.radConfirmable.TabIndex = 8;
            this.radConfirmable.Text = "Confirmable";
            this.radConfirmable.UseVisualStyleBackColor = true;
            this.radConfirmable.CheckedChanged += new System.EventHandler(this.radConfirmable_CheckedChanged);
            // 
            // lblPort
            // 
            this.lblPort.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lblPort.AutoSize = true;
            this.lblPort.Location = new System.Drawing.Point(20, 86);
            this.lblPort.Name = "lblPort";
            this.lblPort.Size = new System.Drawing.Size(26, 13);
            this.lblPort.TabIndex = 3;
            this.lblPort.Text = "Port";
            // 
            // cboPort
            // 
            this.cboPort.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cboPort.FormattingEnabled = true;
            this.cboPort.Items.AddRange(new object[] {
            "5683",
            "5684"});
            this.cboPort.Location = new System.Drawing.Point(23, 102);
            this.cboPort.Name = "cboPort";
            this.cboPort.Size = new System.Drawing.Size(83, 21);
            this.cboPort.TabIndex = 4;
            this.cboPort.SelectedIndexChanged += new System.EventHandler(this.cboPort_SelectedIndexChanged);
            // 
            // lblTimeOut
            // 
            this.lblTimeOut.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lblTimeOut.AutoSize = true;
            this.lblTimeOut.Location = new System.Drawing.Point(19, 139);
            this.lblTimeOut.Name = "lblTimeOut";
            this.lblTimeOut.Size = new System.Drawing.Size(81, 13);
            this.lblTimeOut.TabIndex = 5;
            this.lblTimeOut.Text = "Time Out (secs)";
            // 
            // txtTimeout
            // 
            this.txtTimeout.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.txtTimeout.Location = new System.Drawing.Point(100, 137);
            this.txtTimeout.Name = "txtTimeout";
            this.txtTimeout.Size = new System.Drawing.Size(38, 20);
            this.txtTimeout.TabIndex = 6;
            this.txtTimeout.Text = "5";
            this.txtTimeout.TextChanged += new System.EventHandler(this.txtTimeout_TextChanged);
            // 
            // pnlEdge
            // 
            this.pnlEdge.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlEdge.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(128)))), ((int)(((byte)(0)))));
            this.pnlEdge.Location = new System.Drawing.Point(834, 126);
            this.pnlEdge.Name = "pnlEdge";
            this.pnlEdge.Size = new System.Drawing.Size(19, 492);
            this.pnlEdge.TabIndex = 22;
            this.pnlEdge.Visible = false;
            // 
            // mnuFunctions
            // 
            this.mnuFunctions.Dock = System.Windows.Forms.DockStyle.None;
            this.mnuFunctions.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuOptions,
            this.mnuDebug,
            this.mnuRefreshDevices,
            this.mnuDevice,
            this.mnuResource,
            this.mnuClearCredentials,
            this.mnuAbout});
            this.mnuFunctions.LayoutStyle = System.Windows.Forms.ToolStripLayoutStyle.VerticalStackWithOverflow;
            this.mnuFunctions.Location = new System.Drawing.Point(457, 468);
            this.mnuFunctions.Name = "mnuFunctions";
            this.mnuFunctions.Size = new System.Drawing.Size(152, 139);
            this.mnuFunctions.TabIndex = 23;
            this.mnuFunctions.Text = "View";
            this.mnuFunctions.MouseLeave += new System.EventHandler(this.mnuFunctions_MouseLeave);
            // 
            // mnuOptions
            // 
            this.mnuOptions.Name = "mnuOptions";
            this.mnuOptions.Size = new System.Drawing.Size(145, 19);
            this.mnuOptions.Text = "Options";
            this.mnuOptions.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mnuOptions.Click += new System.EventHandler(this.mnuOptions_Click);
            // 
            // mnuDebug
            // 
            this.mnuDebug.Name = "mnuDebug";
            this.mnuDebug.Size = new System.Drawing.Size(145, 19);
            this.mnuDebug.Text = "Debug";
            this.mnuDebug.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mnuDebug.Click += new System.EventHandler(this.mnuDebug_Click);
            // 
            // mnuRefreshDevices
            // 
            this.mnuRefreshDevices.Name = "mnuRefreshDevices";
            this.mnuRefreshDevices.Size = new System.Drawing.Size(145, 19);
            this.mnuRefreshDevices.Text = "Refresh Device List";
            this.mnuRefreshDevices.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mnuRefreshDevices.Click += new System.EventHandler(this.mnuRefreshDevices_Click);
            // 
            // mnuDevice
            // 
            this.mnuDevice.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuRefreshResources});
            this.mnuDevice.Name = "mnuDevice";
            this.mnuDevice.Size = new System.Drawing.Size(145, 19);
            this.mnuDevice.Text = "Device";
            this.mnuDevice.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mnuDevice.Click += new System.EventHandler(this.mnuDevice_Click);
            // 
            // mnuRefreshResources
            // 
            this.mnuRefreshResources.Name = "mnuRefreshResources";
            this.mnuRefreshResources.Size = new System.Drawing.Size(169, 22);
            this.mnuRefreshResources.Text = "Refresh Resources";
            this.mnuRefreshResources.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mnuRefreshResources.Click += new System.EventHandler(this.mnuRefreshResources_Click);
            // 
            // mnuResource
            // 
            this.mnuResource.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuResourceGet,
            this.mnuResourcePut,
            this.mnuResourceObserve,
            this.mnuResourceDelete});
            this.mnuResource.Name = "mnuResource";
            this.mnuResource.Size = new System.Drawing.Size(145, 19);
            this.mnuResource.Text = "Resource";
            this.mnuResource.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mnuResource.Click += new System.EventHandler(this.mnuResource_Click);
            // 
            // mnuResourceGet
            // 
            this.mnuResourceGet.Name = "mnuResourceGet";
            this.mnuResourceGet.Size = new System.Drawing.Size(117, 22);
            this.mnuResourceGet.Text = "Get";
            this.mnuResourceGet.Click += new System.EventHandler(this.mnuResourceGet_Click);
            // 
            // mnuResourcePut
            // 
            this.mnuResourcePut.Name = "mnuResourcePut";
            this.mnuResourcePut.Size = new System.Drawing.Size(117, 22);
            this.mnuResourcePut.Text = "Put";
            this.mnuResourcePut.Click += new System.EventHandler(this.mnuResourcePut_Click);
            // 
            // mnuResourceObserve
            // 
            this.mnuResourceObserve.Name = "mnuResourceObserve";
            this.mnuResourceObserve.Size = new System.Drawing.Size(117, 22);
            this.mnuResourceObserve.Text = "Observe";
            this.mnuResourceObserve.Click += new System.EventHandler(this.mnuResourceObserve_Click);
            // 
            // mnuResourceDelete
            // 
            this.mnuResourceDelete.Name = "mnuResourceDelete";
            this.mnuResourceDelete.Size = new System.Drawing.Size(117, 22);
            this.mnuResourceDelete.Text = "Delete";
            this.mnuResourceDelete.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mnuResourceDelete.Click += new System.EventHandler(this.mnuResourceDelete_Click);
            // 
            // mnuClearCredentials
            // 
            this.mnuClearCredentials.Name = "mnuClearCredentials";
            this.mnuClearCredentials.Size = new System.Drawing.Size(145, 19);
            this.mnuClearCredentials.Text = "Clear Credentials";
            this.mnuClearCredentials.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mnuClearCredentials.Click += new System.EventHandler(this.mnuClearCredentials_Click);
            // 
            // mnuAbout
            // 
            this.mnuAbout.Name = "mnuAbout";
            this.mnuAbout.Size = new System.Drawing.Size(145, 19);
            this.mnuAbout.Text = "About HDK CoAP Client";
            this.mnuAbout.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.mnuAbout.Click += new System.EventHandler(this.mnuAbout_Click);
            // 
            // pnlResults
            // 
            this.pnlResults.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.pnlResults.Controls.Add(this.btnCloseResults);
            this.pnlResults.Controls.Add(this.txtResult);
            this.pnlResults.Location = new System.Drawing.Point(99, 39);
            this.pnlResults.Name = "pnlResults";
            this.pnlResults.Size = new System.Drawing.Size(443, 454);
            this.pnlResults.TabIndex = 24;
            this.pnlResults.Visible = false;
            // 
            // btnCloseResults
            // 
            this.btnCloseResults.FlatAppearance.BorderSize = 0;
            this.btnCloseResults.FlatAppearance.MouseDownBackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.btnCloseResults.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnCloseResults.Image = global::HdkClient.Properties.Resources.Close_16x;
            this.btnCloseResults.Location = new System.Drawing.Point(416, 3);
            this.btnCloseResults.Name = "btnCloseResults";
            this.btnCloseResults.Size = new System.Drawing.Size(21, 20);
            this.btnCloseResults.TabIndex = 27;
            this.btnCloseResults.UseVisualStyleBackColor = true;
            this.btnCloseResults.Click += new System.EventHandler(this.btnCloseResults_Click);
            // 
            // txtResult
            // 
            this.txtResult.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left)));
            this.txtResult.Location = new System.Drawing.Point(3, 3);
            this.txtResult.Multiline = true;
            this.txtResult.Name = "txtResult";
            this.txtResult.ReadOnly = true;
            this.txtResult.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtResult.Size = new System.Drawing.Size(409, 451);
            this.txtResult.TabIndex = 20;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(23, 353);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(113, 23);
            this.button1.TabIndex = 25;
            this.button1.Text = "Establish Session";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click_1);
            // 
            // txtLog
            // 
            this.txtLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtLog.Location = new System.Drawing.Point(22, 475);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtLog.Size = new System.Drawing.Size(650, 117);
            this.txtLog.TabIndex = 26;
            this.txtLog.WordWrap = false;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(23, 382);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(113, 23);
            this.button2.TabIndex = 27;
            this.button2.Text = "Terminiate Session";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // frmNewMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(853, 616);
            this.Controls.Add(this.txtLog);
            this.Controls.Add(this.pnlResults);
            this.Controls.Add(this.pnlEdge);
            this.Controls.Add(this.pnlOptions);
            this.Controls.Add(this.treeDevices);
            this.Controls.Add(this.mnuFunctions);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.mnuFunctions;
            this.MinimumSize = new System.Drawing.Size(869, 655);
            this.Name = "frmNewMain";
            this.Text = "Starfish HDK Client";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmNewMain_FormClosing);
            this.Load += new System.EventHandler(this.frmNewMain_Load);
            this.ResizeEnd += new System.EventHandler(this.frmNewMain_ResizeEnd);
            this.pnlOptions.ResumeLayout(false);
            this.pnlOptions.PerformLayout();
            this.grpDiscoverModel.ResumeLayout(false);
            this.grpDiscoverModel.PerformLayout();
            this.grpNetworkInterface.ResumeLayout(false);
            this.grpNetworkInterface.PerformLayout();
            this.grpResponseModel.ResumeLayout(false);
            this.grpResponseModel.PerformLayout();
            this.mnuFunctions.ResumeLayout(false);
            this.mnuFunctions.PerformLayout();
            this.pnlResults.ResumeLayout(false);
            this.pnlResults.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TreeView treeDevices;
        private System.Windows.Forms.Panel pnlOptions;
        private System.Windows.Forms.GroupBox grpNetworkInterface;
        private System.Windows.Forms.RadioButton radGateway;
        private System.Windows.Forms.GroupBox grpResponseModel;
        private System.Windows.Forms.RadioButton radNonConfirmable;
        private System.Windows.Forms.RadioButton radConfirmable;
        private System.Windows.Forms.Label lblPort;
        private System.Windows.Forms.ComboBox cboPort;
        private System.Windows.Forms.Label lblTimeOut;
        private System.Windows.Forms.TextBox txtTimeout;
        private System.Windows.Forms.Panel pnlEdge;
        private System.Windows.Forms.MenuStrip mnuFunctions;
        private System.Windows.Forms.ToolStripMenuItem mnuOptions;
        private System.Windows.Forms.Button btnCloseOptions;
        private System.Windows.Forms.ToolStripMenuItem mnuDebug;
        private System.Windows.Forms.Panel pnlResults;
        private System.Windows.Forms.Button btnCloseResults;
        private System.Windows.Forms.TextBox txtResult;
        private System.Windows.Forms.ToolStripMenuItem mnuResource;
        private System.Windows.Forms.ToolStripMenuItem mnuResourceGet;
        private System.Windows.Forms.ToolStripMenuItem mnuResourcePut;
        private System.Windows.Forms.ToolStripMenuItem mnuResourceObserve;
        private System.Windows.Forms.ToolStripMenuItem mnuRefreshDevices;
        private System.Windows.Forms.ToolStripMenuItem mnuResourceDelete;
        private System.Windows.Forms.ToolStripMenuItem mnuClearCredentials;
        private System.Windows.Forms.GroupBox grpDiscoverModel;
        private System.Windows.Forms.RadioButton radDemandDiscover;
        private System.Windows.Forms.RadioButton radAutoDiscover;
        private System.Windows.Forms.ToolStripMenuItem mnuAbout;
        private System.Windows.Forms.RadioButton radSimulator;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ToolStripMenuItem mnuDevice;
        private System.Windows.Forms.ToolStripMenuItem mnuRefreshResources;
        private System.Windows.Forms.Button btnConnectionSettings;
        private System.Windows.Forms.TextBox txtLog;
        private System.Windows.Forms.Button button2;
    }
}