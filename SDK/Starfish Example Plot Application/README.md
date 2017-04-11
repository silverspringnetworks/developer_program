# Silver Spring Networks Starfish Data Platform Reference APP

To run this application, just open `index.html` in the browser.

This sample application demonstrates how to use `Starfish JS SDK` to build a simple web application to fetch devices and observations from *Starfish Data Platform*.

To use the sample application you will need clientId/secret issued by *Starfish Data Platform*.

Fill in clientId/secret and click on 'Connect' to fetch devices and choose a device to plot temperature observations for that device.

Fill in  the solution name as either `StudioSimulation` or `sandbox`
Solutions are a logical grouping of devices and their associated data. Solutions are created automatically when a device is created using that solution.
* Devices created via studio simulation will be part of the solution named `StudioSimulation`
* Devices purchased via the development portal will be part of the solution named `sandbox`

Observations must be of format:
```json
{
  "observations": [
    {
      "temperature": -31.4215,
      "timestamp": "2017-03-21T22:04:08.667Z"
    }
  ]
}
```
