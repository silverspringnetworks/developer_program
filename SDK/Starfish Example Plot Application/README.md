# Silver Spring Networks Starfish JS SDK Reference APP

To run this application, just open `index.html` in the browser.

This sample application demonstrates how to use `Starfish JS SDK` to build a simple web application to fetch devices and observations from *Starfish Data Platform*.

To use the sample application you will need clientId/secret issued by *Starfish Data Platform*.

Fill in clientId/secret and click on 'Connect' to fetch devices and choose a device to plot temperature observations for that device.

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
