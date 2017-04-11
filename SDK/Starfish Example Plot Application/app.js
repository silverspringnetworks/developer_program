/* Copyright (c) Silver Spring Networks, Inc.
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the ""Software""), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Silver Spring Networks, Inc.
 * shall not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization from
 * Silver Spring Networks, Inc.
 */
var config = {};
var timer;
var sf;

/** Initialize the application */
$(document).ready(function() {
  config.type = 'line';
  config.options = {};
  config.data = {labels:[], datasets:[]};
  config.data.datasets.push({
    label: "Temperature",
    backgroundColor: 'rgb(75, 192, 192)',
    borderColor: 'rgb(75, 192, 192)',
    fill: false,
    lineTension: 0,
    data:[]
  });
  config.options.animation = false;
  var ctx = document.getElementById("observations").getContext("2d");
  window.chart = new Chart(ctx, config);
  $(".js-form").on("submit", handleConnect)
});

/** Connect to the starfish data platform
 *
 * Called when the 'Connect' button is pressed. This handler will initialize
 * the StarfishService class using the user provided clientId and secret.  It
 * then loads devices for the specified solution
 */
function handleConnect(event) {
  event.preventDefault();
  $(".message.is-showing").removeClass("is-showing");
  var clientId = document.getElementById("clientId").value;
  var clientSecret = document.getElementById("clientSecret").value;
  var solution = document.getElementById("solution").value;


  clearGraph();

  console.log("Attempting to connect with:", clientId, clientSecret)
  var options = {solution, credentials: {clientId, clientSecret}};
  sf = new StarfishService(options)
  $("#deviceTable tbody tr").remove()
  sf.getDevices(function (err,result) {
    if(err) {
      switch(err.statusCode) {
        case 404:
          $(".error-no-devices").addClass("is-showing");
          break;
        case 400:
          $(".error-bad-request").addClass("is-showing").find("h3").text(err.error.message);

          break;
        default:
          $(".error-other").addClass("is-showing");
          break;
      }
      console.log(err);
    }
    else if(result && result.length == 0) {
      console.log('No results...')
      $(".error-no-devices").addClass("is-showing");
    } else {
      console.log(err,result)
      var tbl = document.getElementById("device-table-body")
      result.forEach( function(device) {
        var row = tbl.insertRow(-1)
        var idCell = row.insertCell(0)
        var macAddressCell = row.insertCell(1)
        row.onclick = function(e) {
          $("#device-table-body tr").removeClass("active");
          $(e.currentTarget).addClass("active");
          selectDevice(device);
        }
        idCell.innerHTML = device.id;
        macAddressCell.innerHTML = "<em>Unknown</em>"
        if(device.provisioningInfo && device.provisioningInfo.ssniMacAddress) {
          macAddressCell.innerHTML = device.provisioningInfo.ssniMacAddress;
        }
      })
    }
  })
}

/** Select a device
 *
 * This handler is called when a device is selected, and calls getObservations
 * to load data for that device every 10 seconds until another device is
 * selected.
 */
function selectDevice(device) {
  clearGraph();
  getObservations(device);
  timer = setInterval(function() {
    getObservations(device);
  }, 10000);
}

/** Load temperature observations for a selected device using the starfish sdk.
 * Plots observations using chartjs.
 */
function getObservations(device) {
  sf.getDeviceObservations(device.id, function(err, response) {
    if(response && response.observations) {
      var newData = [];
      var newLabels = [];
      response.observations.forEach( function(obs, i) {
        var ts = moment(obs.timestamp, moment.ISO_8601).format('hh:mm:ss');
        newLabels.unshift(ts);
        newData.unshift({x: obs.timestamp, y: obs.temperature});
      })
      config.data.labels = newLabels;
      config.data.datasets[0].data = newData;
      window.chart.update();
    }
  })
}

/** Clear the chartjs graph and stop active timers */
function clearGraph() {
  if(timer) {
    clearInterval(timer);
  }
  config.data.labels = []
  config.data.datasets[0].data = []
  window.chart.update();
}
