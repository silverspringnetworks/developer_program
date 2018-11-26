namespace HdkClient
{
    partial class HdkSplash
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(HdkSplash));
            this.radAccept = new System.Windows.Forms.RadioButton();
            this.radDecline = new System.Windows.Forms.RadioButton();
            this.transparentPictureBox1 = new HdkClient.TransparentPictureBox();
            this.tpicSplash = new HdkClient.TransparentPictureBox();
            this.trtdbLegal = new HdkClient.TransparentRichTextBox();
            this.chkNoAccept = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.transparentPictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.tpicSplash)).BeginInit();
            this.SuspendLayout();
            // 
            // radAccept
            // 
            this.radAccept.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.radAccept.AutoSize = true;
            this.radAccept.Location = new System.Drawing.Point(140, 590);
            this.radAccept.Name = "radAccept";
            this.radAccept.Size = new System.Drawing.Size(59, 17);
            this.radAccept.TabIndex = 2;
            this.radAccept.TabStop = true;
            this.radAccept.Text = "Accept";
            this.radAccept.UseVisualStyleBackColor = true;
            this.radAccept.CheckedChanged += new System.EventHandler(this.radAccept_CheckedChanged);
            // 
            // radDecline
            // 
            this.radDecline.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.radDecline.AutoSize = true;
            this.radDecline.Location = new System.Drawing.Point(218, 590);
            this.radDecline.Name = "radDecline";
            this.radDecline.Size = new System.Drawing.Size(61, 17);
            this.radDecline.TabIndex = 3;
            this.radDecline.TabStop = true;
            this.radDecline.Text = "Decline";
            this.radDecline.UseVisualStyleBackColor = true;
            this.radDecline.CheckedChanged += new System.EventHandler(this.radDecline_CheckedChanged);
            // 
            // transparentPictureBox1
            // 
            this.transparentPictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("transparentPictureBox1.Image")));
            this.transparentPictureBox1.Location = new System.Drawing.Point(271, 154);
            this.transparentPictureBox1.Name = "transparentPictureBox1";
            this.transparentPictureBox1.Size = new System.Drawing.Size(500, 414);
            this.transparentPictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.transparentPictureBox1.TabIndex = 7;
            this.transparentPictureBox1.TabStop = false;
            // 
            // tpicSplash
            // 
            this.tpicSplash.BackColor = System.Drawing.Color.Transparent;
            this.tpicSplash.Image = ((System.Drawing.Image)(resources.GetObject("tpicSplash.Image")));
            this.tpicSplash.Location = new System.Drawing.Point(1, 1);
            this.tpicSplash.Name = "tpicSplash";
            this.tpicSplash.Size = new System.Drawing.Size(1031, 147);
            this.tpicSplash.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.tpicSplash.TabIndex = 6;
            this.tpicSplash.TabStop = false;
            // 
            // trtdbLegal
            // 
            this.trtdbLegal.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.trtdbLegal.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.trtdbLegal.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.trtdbLegal.Location = new System.Drawing.Point(139, 206);
            this.trtdbLegal.Name = "trtdbLegal";
            this.trtdbLegal.Size = new System.Drawing.Size(751, 373);
            this.trtdbLegal.TabIndex = 4;
            this.trtdbLegal.Text = "";
            // 
            // chkNoAccept
            // 
            this.chkNoAccept.AutoSize = true;
            this.chkNoAccept.Location = new System.Drawing.Point(877, 594);
            this.chkNoAccept.Name = "chkNoAccept";
            this.chkNoAccept.Size = new System.Drawing.Size(144, 17);
            this.chkNoAccept.TabIndex = 9;
            this.chkNoAccept.Text = "Don\'t show me this again";
            this.chkNoAccept.UseVisualStyleBackColor = true;
            this.chkNoAccept.CheckedChanged += new System.EventHandler(this.chkNoAccept_CheckedChanged);
            // 
            // HdkSplash
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.ClientSize = new System.Drawing.Size(1033, 624);
            this.ControlBox = false;
            this.Controls.Add(this.chkNoAccept);
            this.Controls.Add(this.transparentPictureBox1);
            this.Controls.Add(this.tpicSplash);
            this.Controls.Add(this.trtdbLegal);
            this.Controls.Add(this.radDecline);
            this.Controls.Add(this.radAccept);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "HdkSplash";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Load += new System.EventHandler(this.HdkSplash_Load);
            this.Shown += new System.EventHandler(this.HdkSplash_Shown);
            ((System.ComponentModel.ISupportInitialize)(this.transparentPictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.tpicSplash)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.RadioButton radAccept;
        private System.Windows.Forms.RadioButton radDecline;
        private TransparentRichTextBox trtdbLegal;
        private TransparentPictureBox tpicSplash;
        private TransparentPictureBox transparentPictureBox1;
        private System.Windows.Forms.CheckBox chkNoAccept;
    }
}