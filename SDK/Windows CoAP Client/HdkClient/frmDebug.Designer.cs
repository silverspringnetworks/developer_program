namespace HdkClient
{
    partial class frmDebug
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
            this.btnViewLog = new System.Windows.Forms.Button();
            this.btnClear = new System.Windows.Forms.Button();
            this.lblLogSize = new System.Windows.Forms.Label();
            this.txtMaxLogSize = new System.Windows.Forms.TextBox();
            this.cboDebugLevel = new System.Windows.Forms.ComboBox();
            this.lblLogLevel = new System.Windows.Forms.Label();
            this.chkEnabled = new System.Windows.Forms.CheckBox();
            this.btnCancel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // btnViewLog
            // 
            this.btnViewLog.Location = new System.Drawing.Point(22, 28);
            this.btnViewLog.Name = "btnViewLog";
            this.btnViewLog.Size = new System.Drawing.Size(75, 23);
            this.btnViewLog.TabIndex = 0;
            this.btnViewLog.Text = "View";
            this.btnViewLog.UseVisualStyleBackColor = true;
            this.btnViewLog.Click += new System.EventHandler(this.btnViewLog_Click);
            // 
            // btnClear
            // 
            this.btnClear.Location = new System.Drawing.Point(22, 57);
            this.btnClear.Name = "btnClear";
            this.btnClear.Size = new System.Drawing.Size(75, 23);
            this.btnClear.TabIndex = 1;
            this.btnClear.Text = "Clear";
            this.btnClear.UseVisualStyleBackColor = true;
            this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
            // 
            // lblLogSize
            // 
            this.lblLogSize.AutoSize = true;
            this.lblLogSize.Location = new System.Drawing.Point(20, 171);
            this.lblLogSize.Name = "lblLogSize";
            this.lblLogSize.Size = new System.Drawing.Size(71, 13);
            this.lblLogSize.TabIndex = 54;
            this.lblLogSize.Text = "Max Log Size";
            // 
            // txtMaxLogSize
            // 
            this.txtMaxLogSize.Location = new System.Drawing.Point(98, 168);
            this.txtMaxLogSize.Name = "txtMaxLogSize";
            this.txtMaxLogSize.Size = new System.Drawing.Size(63, 20);
            this.txtMaxLogSize.TabIndex = 53;
            this.txtMaxLogSize.TextChanged += new System.EventHandler(this.txtMaxLogSize_TextChanged);
            // 
            // cboDebugLevel
            // 
            this.cboDebugLevel.FormattingEnabled = true;
            this.cboDebugLevel.Location = new System.Drawing.Point(98, 141);
            this.cboDebugLevel.Name = "cboDebugLevel";
            this.cboDebugLevel.Size = new System.Drawing.Size(96, 21);
            this.cboDebugLevel.TabIndex = 52;
            this.cboDebugLevel.SelectedIndexChanged += new System.EventHandler(this.cboDebugLevel_SelectedIndexChanged);
            // 
            // lblLogLevel
            // 
            this.lblLogLevel.AutoSize = true;
            this.lblLogLevel.Location = new System.Drawing.Point(20, 143);
            this.lblLogLevel.Name = "lblLogLevel";
            this.lblLogLevel.Size = new System.Drawing.Size(68, 13);
            this.lblLogLevel.TabIndex = 51;
            this.lblLogLevel.Text = "Debug Level";
            // 
            // chkEnabled
            // 
            this.chkEnabled.AutoSize = true;
            this.chkEnabled.Location = new System.Drawing.Point(22, 114);
            this.chkEnabled.Name = "chkEnabled";
            this.chkEnabled.Size = new System.Drawing.Size(65, 17);
            this.chkEnabled.TabIndex = 55;
            this.chkEnabled.Text = "Enabled";
            this.chkEnabled.UseVisualStyleBackColor = true;
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(23, 28);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 56;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // frmDebug
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(297, 214);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.chkEnabled);
            this.Controls.Add(this.lblLogSize);
            this.Controls.Add(this.txtMaxLogSize);
            this.Controls.Add(this.cboDebugLevel);
            this.Controls.Add(this.lblLogLevel);
            this.Controls.Add(this.btnClear);
            this.Controls.Add(this.btnViewLog);
            this.Name = "frmDebug";
            this.ShowIcon = false;
            this.Text = "Debug Log";
            this.Load += new System.EventHandler(this.frmDebug_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnViewLog;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.Label lblLogSize;
        private System.Windows.Forms.TextBox txtMaxLogSize;
        private System.Windows.Forms.ComboBox cboDebugLevel;
        private System.Windows.Forms.Label lblLogLevel;
        private System.Windows.Forms.CheckBox chkEnabled;
        private System.Windows.Forms.Button btnCancel;
    }
}