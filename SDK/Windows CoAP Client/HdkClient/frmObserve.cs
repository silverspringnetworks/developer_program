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
using EXILANT.Labs.CoAP.Settings;
using EXILANT.Labs.CoAP.Channels;
using EXILANT.Labs.CoAP.Message;
using EXILANT.Labs.CoAP.Helpers;
using System.Windows.Forms;

namespace HdkClient
{
    public delegate void ControlStringConsumer(Control control, string text);  // defines a delegate type

    public partial class frmObserve : Form
    {
        private CoApObserverGateway __Observer = null;

        private string __DeviceName = "";
        private string __URI = "";
        public string DeviceName
        {
            get { return __DeviceName; }
            set { __DeviceName = value; }
        }
        public string URI
        {
            get { return __URI; }
            set { __URI = value; }
        }

        public frmObserve()
        {
            InitializeComponent();
        }

        private void frmObserve_Load(object sender, EventArgs e)
        {
            txtDevice.Text = __DeviceName;
            txtURI.Text = __URI;
            pnlResults.Visible = true;
        }

        private void btnGet_Click(object sender, EventArgs e)
        {
            if (btnStartStop.Text == "Start")
            {
                ObserveResource();
            }
            else
            {
                TerminateObserve();
            }
        }
        /// <summary>
        /// Observe the resource closest to the clicked location.
        /// </summary>
        private void ObserveResource()
        {
            // Reset the result text.
            txtResult.Text = "Starting\r\n";

            if (__Observer == null)
            {
                __Observer = new CoApObserverGateway();
                __Observer.ObserveResponseReceived += new CoApObserverGateway.ObserveResponseHandler(HandleObserveResponse);
            }

            // Drill into the device tree and fetch the related device name.
            __Observer.IpAddress = __DeviceName;
            __Observer.ServerPort = GatewaySettings.Instance.CoApPort;
            // Use the resource property for the target resource.
            __Observer.URI = txtURI.Text;
            btnStartStop.Text = "Stop";
            __Observer.StartObserving();
        }
        /// <summary>
        /// Process GET responses related to observe.
        /// </summary>
        /// <param name="coapResp">the CoAPResponse object that we are going to report</param>
        private void HandleObserveResponse(CoAPResponse coapResp)
        {
            string toAdd = "";

            try
            {
                toAdd = coapResp.ToString();
                // Setting the text in a method that may have to be Invoked.
                SetText(txtResult, txtResult.Text + toAdd);
            }
            catch(Exception oops)
            {
                FileLogger.Write(oops.Message);
                FileLogger.Write(oops.StackTrace);
            }

        }

        /// <summary>
        /// Shuts down the current observe operation.
        /// Sets start/stop button text
        /// </summary>
        private void TerminateObserve()
        {
            // Set the button text
            btnStartStop.Text = "Start";
            // Issue a GET that changes the OBSERVE option to stop observing the target resource
            __Observer.TerminateObserving();
            txtResult.Text += "\r\nStopping\r\n";
        }
        /// <summary>
        /// Add text to the progress textbox.
        /// Note that because the result to add is coming in on a different thread, we have to 
        /// determine whether to do an invoke on this.
        /// </summary>
        /// <param name="control"></param>
        /// <param name="text"></param>
        public void SetText(Control control, string text)
        {
            if (control.InvokeRequired)
            {
                control.Invoke(new ControlStringConsumer(SetText), new object[] { control, text });  // invoking itself
            }
            else
            {
                // the "functional part", executing only on the main thread
                control.Text = text;            
                // If we have already stopped observing, this allows us to capture the final response.
                // Now, we can stop catching responses.
                if (!__Observer.Observing)     
                {
                    // Undo the listening callback
                    __Observer.ObserveResponseReceived -= new CoApObserverGateway.ObserveResponseHandler(HandleObserveResponse);
                    txtResult.Text += "\r\nStopped";
                }
            }
        }
        /// <summary>
        /// Handle the length of the text to display.
        /// We are rolling the response text so that we don't get too much text.
        /// The top part of the textbox gets trimmed off whenever we exceed
        /// the maximum allowed text length.
        /// </summary>
        /// <param name="sender">the calling object</param>
        /// <param name="e">related event attributes</param>
        private void txtResult_TextChanged(object sender, EventArgs e)
        {
            // For the moment, we are arbitrarily restricting the text length to 10000 bytes.
            if (txtResult.Text.Length > 10000)
            {
                // Always show the last 10000 bytes.  Truncate the text at the front.
                txtResult.Text = txtResult.Text.Substring(txtResult.Text.Length-10000);
            }

        }
    }
}
