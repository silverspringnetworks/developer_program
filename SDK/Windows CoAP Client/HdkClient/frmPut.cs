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
using EXILANT.Labs.CoAP.Settings;
using EXILANT.Labs.CoAP.Channels;
using EXILANT.Labs.CoAP.Message;
using EXILANT.Labs.CoAP.Helpers;
using System.IO;
namespace HdkClient
{
    /// <summary>
    /// The 
    /// </summary>
    public partial class frmPut : Form
    {
        private string __DeviceName = "";
        private string __ResourceName = "";
        private byte[] __Payload;
        /// <summary>
        /// The list of supported content types for the PUT
        /// </summary>
        private enum ContentType
        {
            TEXT_PLAIN = 0,
            APPLICATION_LINK_FORMAT = 40,
            APPLICATION_XML = 41,
            APPLICATION_OCTET_STREAM = 42,
            APPLICATION_EXI = 47,
            APPLICATION_JSON = 50
        }
    public frmPut()
        {
            InitializeComponent();
        }
        /// <summary>
        /// The name of the target device
        /// </summary>
        public string DeviceName
        {
            get { return __DeviceName; }
            set { __DeviceName = value; }
        }
        /// <summary>
        /// The resource to receive the PUT.
        /// </summary>
        public string ResourceName
        {
            get { return __ResourceName; }
            set { __ResourceName = value; }
        }
        private void frmPut_Load(object sender, EventArgs e)
        {
            // Commenting out the unsupported types for the moment.

            //cboContentType.Items.Add(ContentType.APPLICATION_EXI);
            //cboContentType.Items.Add(ContentType.APPLICATION_JSON);
            //cboContentType.Items.Add(ContentType.APPLICATION_LINK_FORMAT);
            cboContentType.Items.Add(ContentType.APPLICATION_OCTET_STREAM);
            //cboContentType.Items.Add(ContentType.APPLICATION_XML);
            cboContentType.Items.Add(ContentType.TEXT_PLAIN);
            cboContentType.SelectedIndex = 0;

            txtDevice.Text = __DeviceName;
            txtURI.Text = __ResourceName;
        }

        private void cboContentType_SelectedIndexChanged(object sender, EventArgs e)
        {
            ComboBox b = (ComboBox)sender;
            string s = b.SelectedItem.ToString();
            
            ContentType t = (ContentType)Enum.Parse(typeof(ContentType), s);
            byte f = (byte)t;
        }

        private void btnPut_Click(object sender, EventArgs e)
        {
            CoApPut put = (CoApPut)NetworkInterface.Instance.ResourcePutter;
            put.IpAddress = txtDevice.Text;
            put.ServerPort = GatewaySettings.Instance.CoApPort;
            // Drill into the device tree and fetch the related resource to fetch.
            put.URI = txtURI.Text;
            if (!(__Payload == null || __Payload.Length == 0))
            {
                put.Payload = new CoAPPayload(__Payload);
            }
            put.Send();
            string result = "";
            try
            {
                result = put.Response.ToString();
                if (put.Response.Payload.Value != null)
                {
                    if (put.Response.Payload.Value.Length != 0)
                    {
                        result += HdkUtils.BytesToHexView(put.Response.Payload.Value);
                    }
                }
            }
            catch
            {
                result = "Unable to retreive response from resource";
            }
            // Show the outcome from the PUT request.
            MessageBox.Show(result);
            Cursor.Current = Cursors.Default;

        }
        /// <summary>
        /// Capture the file contents.
        /// </summary>
        /// <param name="sender">the object dropped from</param>
        /// <param name="e">the arguments containing the data content</param>
        private void txtToPut_DragDrop(object sender, DragEventArgs e)
        {
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
            foreach (string file in files) Console.WriteLine(file);
            if (File.Exists(files[0]))
            {
                __Payload = File.ReadAllBytes(files[0]);
                txtToPut.Text = HdkUtils.BytesToHexView(__Payload);
            }
        }
        /// <summary>
        /// Confirm that we have data to drop.
        /// Either accept the drop or not depending on the drag content.
        /// </summary>
        /// <param name="sender">the object dragged from</param>
        /// <param name="e">the arguments containing the data content</param>
        private void txtToPut_DragEnter(object sender, DragEventArgs e)
        {
            // Confirm that data is present.
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
                e.Effect = DragDropEffects.All;     // Allow drop related data to occur
            else
                e.Effect = DragDropEffects.None;    // Ignore any drop (mouse up) since there is nothing to process
        }
    }
}
