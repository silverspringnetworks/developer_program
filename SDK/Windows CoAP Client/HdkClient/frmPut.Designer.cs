namespace HdkClient
{
    partial class frmPut
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
            this.cboContentType = new System.Windows.Forms.ComboBox();
            this.lblContentType = new System.Windows.Forms.Label();
            this.txtToPut = new System.Windows.Forms.TextBox();
            this.btnPut = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.lblDevice = new System.Windows.Forms.Label();
            this.txtDevice = new System.Windows.Forms.TextBox();
            this.lblURI = new System.Windows.Forms.Label();
            this.txtURI = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // cboContentType
            // 
            this.cboContentType.FormattingEnabled = true;
            this.cboContentType.Location = new System.Drawing.Point(486, 43);
            this.cboContentType.Name = "cboContentType";
            this.cboContentType.Size = new System.Drawing.Size(121, 21);
            this.cboContentType.TabIndex = 0;
            this.cboContentType.SelectedIndexChanged += new System.EventHandler(this.cboContentType_SelectedIndexChanged);
            // 
            // lblContentType
            // 
            this.lblContentType.AutoSize = true;
            this.lblContentType.Location = new System.Drawing.Point(483, 27);
            this.lblContentType.Name = "lblContentType";
            this.lblContentType.Size = new System.Drawing.Size(71, 13);
            this.lblContentType.TabIndex = 1;
            this.lblContentType.Text = "Content Type";
            // 
            // txtToPut
            // 
            this.txtToPut.AllowDrop = true;
            this.txtToPut.Location = new System.Drawing.Point(12, 65);
            this.txtToPut.Multiline = true;
            this.txtToPut.Name = "txtToPut";
            this.txtToPut.Size = new System.Drawing.Size(449, 287);
            this.txtToPut.TabIndex = 2;
            this.txtToPut.DragDrop += new System.Windows.Forms.DragEventHandler(this.txtToPut_DragDrop);
            this.txtToPut.DragEnter += new System.Windows.Forms.DragEventHandler(this.txtToPut_DragEnter);
            // 
            // btnPut
            // 
            this.btnPut.Location = new System.Drawing.Point(479, 300);
            this.btnPut.Name = "btnPut";
            this.btnPut.Size = new System.Drawing.Size(128, 23);
            this.btnPut.TabIndex = 3;
            this.btnPut.Text = "PUT";
            this.btnPut.UseVisualStyleBackColor = true;
            this.btnPut.Click += new System.EventHandler(this.btnPut_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(479, 329);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(128, 23);
            this.btnCancel.TabIndex = 32;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // lblDevice
            // 
            this.lblDevice.AutoSize = true;
            this.lblDevice.Location = new System.Drawing.Point(9, 15);
            this.lblDevice.Name = "lblDevice";
            this.lblDevice.Size = new System.Drawing.Size(44, 13);
            this.lblDevice.TabIndex = 36;
            this.lblDevice.Text = "Device:";
            // 
            // txtDevice
            // 
            this.txtDevice.Location = new System.Drawing.Point(59, 12);
            this.txtDevice.Name = "txtDevice";
            this.txtDevice.Size = new System.Drawing.Size(181, 20);
            this.txtDevice.TabIndex = 35;
            // 
            // lblURI
            // 
            this.lblURI.AutoSize = true;
            this.lblURI.Location = new System.Drawing.Point(24, 38);
            this.lblURI.Name = "lblURI";
            this.lblURI.Size = new System.Drawing.Size(29, 13);
            this.lblURI.TabIndex = 34;
            this.lblURI.Text = "URI:";
            // 
            // txtURI
            // 
            this.txtURI.Location = new System.Drawing.Point(59, 38);
            this.txtURI.Name = "txtURI";
            this.txtURI.Size = new System.Drawing.Size(340, 20);
            this.txtURI.TabIndex = 33;
            // 
            // frmPut
            // 
            this.AcceptButton = this.btnPut;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(626, 364);
            this.Controls.Add(this.lblDevice);
            this.Controls.Add(this.txtDevice);
            this.Controls.Add(this.lblURI);
            this.Controls.Add(this.txtURI);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnPut);
            this.Controls.Add(this.txtToPut);
            this.Controls.Add(this.lblContentType);
            this.Controls.Add(this.cboContentType);
            this.Name = "frmPut";
            this.ShowIcon = false;
            this.Text = "PUT";
            this.Load += new System.EventHandler(this.frmPut_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox cboContentType;
        private System.Windows.Forms.Label lblContentType;
        private System.Windows.Forms.TextBox txtToPut;
        private System.Windows.Forms.Button btnPut;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Label lblDevice;
        private System.Windows.Forms.TextBox txtDevice;
        private System.Windows.Forms.Label lblURI;
        private System.Windows.Forms.TextBox txtURI;
    }
}