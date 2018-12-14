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
using System.IO;
using System.Diagnostics;

namespace HdkClient
{
    /// <summary>
    /// Debug form.
    /// This is where we handle different debug levels, clearing out the old log, etc.
    /// </summary>
    public partial class frmDebug : Form
    {
        public frmDebug()
        {
            InitializeComponent();
        }

        /// <summary>
        /// When the clear button is triggered, the current log is deleted.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnClear_Click(object sender, EventArgs e)
        {
            try
            {
                File.Delete(FileLogger.Path);
                StreamWriter s = File.CreateText(FileLogger.Path);
                s.Close();
            }
            catch
            {
            }
        }
        /// <summary>
        /// Form initial load.
        /// Current debug logger values are set on the UI.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void frmDebug_Load(object sender, EventArgs e)
        {
            btnViewLog.BringToFront();
            btnCancel.SendToBack();
            cboDebugLevel.DataSource = Enum.GetValues(typeof(FileLogger.LogLevel));
            FileLogger.getit();
            txtMaxLogSize.Text = FileLogger.MaxFileSize.ToString();
            chkEnabled.Checked = FileLogger.Enabled;
        }
        /// <summary>
        /// Debug level has been changed.
        /// Set the underlying debug logger level to match.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void cboDebugLevel_SelectedIndexChanged(object sender, EventArgs e)
        {
            FileLogger.Level = (FileLogger.LogLevel)cboDebugLevel.SelectedValue;
            FileLogger.Enabled = (FileLogger.Level != FileLogger.LogLevel.Off) ? true : false;
        }
        /// <summary>
        /// When View is clicked we spawn another process to read the log.
        /// The default file type is .txt, thus typiclly, notepad is fired up.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void btnViewLog_Click(object sender, EventArgs e)
        {
            try
            {
                Process x = Process.Start(FileLogger.Path);
                x.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show("Could not open debug log; " + ex.Message,
                                "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        /// <summary>
        /// This value is used to set the size of the log.
        /// Once this size is hit, the log is cleaned up and re-filled.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void txtMaxLogSize_TextChanged(object sender, EventArgs e)
        {
            try
            {
                FileLogger.MaxFileSize = Convert.ToInt64(txtMaxLogSize.Text);
            }
            catch
            {
                MessageBox.Show("Could not set debug log size to specified value");
                txtMaxLogSize.SelectAll();
                txtMaxLogSize.Focus();
                return;
            }

        }
    }
}
