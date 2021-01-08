## Java CoAP Client
### Itron Developer Program SDK

### Current Version Info
Current version is 1.4.6.
See the changes.md file for details.

#### Overview
A component of the Silver Sprint Networks developer program, the Java CoAP client is a standalone
command line utiltiy that can be used to communicate with any SSN based CoAP based IoT device;
any of the Hardware Developer Kits available for purchase on the Starfish Developer Portal.

The Java CoAP Client is packaged as a jar and takes a configuration file that defines network
parameters and the IoT device as well as command line options. You can run the client on Linux, Windows, and MacOS.

To run the client use a command of this form in a command shell (in this case to get the list of options):
```$xslt
java -jar sdkcoapclient-1.4.3.one-jar.jar --help
```
To run the client with a config file use the following command form (in this case to issue the CoAP
activity defined by the coap.json file):
```$xslt
java -jar sdkcoapclient-1.4.3.one-jar.jar --conf coap.json
```

#### Prerequisites 
You will need the following tools installed on your system:
- Java 8
- Maven (version 3+)
- Git utilities

#### Checklist
The rough outline of steps needed to builr the Java CoAP Client
- Install the prerequisites
- Build the nCoAP components
- Build the Java CoAP Client
- Determine your Starfish account ClientId and ClientSecret
- Determine the Starfish device id of your Silver Spring IoT device
- Setup CoAP configuration files to define you CoAP activities
- Use Java CoAP Client

#### Build nCoAP Components
The nCoAP (which is a Java implementation of the CoAP protocal standard) is available on GitHub.
First, clone the GitHub repository. Point your browser to the following URL:
`https://github.com/okleine/nCoAP`.

Clone the nCoAP repository on your local system. For example, if you clone nCoAP at `C:\projects`
than the cloning process will create a sub-directory called `nCoAP`. Change your working directory
to `C:\projects\nCoAP` and issue the following maven command. Note that we recommend that you do
not run the tests as there appears to be timing sensitivity on some systems and the tests may fail.

```$xslt
mvn clean install -Dmaven.test.skip=true
```
This will build the necessary nCoAP jars and place them in your maven repository (they are dependencies
used by the Java CoAP Client).

#### Build Java CoAP Client
You have probably already cloned the developer program GitHub repository. If not, now's the time!

The Java CoAP Client is available on GitHub as part of the developer_program repository.
First, clone the GitHub repository. Point your browser to the following URL:
`https://github.com/silverspringnetworks/developer_program`.

Clone the nCoAP repository on your local system. For example, if you clone to `C:\projects`
than the cloning process will create a sub-directory called `developer_program`.
Change your working directory
to `C:\projects\developer_program\SDK\Java CoAP Client` and issue the following maven command:

```$xslt
mvn clean install
```

This will build the Java CoAP Client. The resultant jar can found in the target subdirectory
(using the sample directory used above):

```$xslt
C:\projects\developer_program\SDK\Java CoAP Client\target\sdkcoapclient-1.4.3.one-jar.jar
```
The jar is built as an all in one jar, it contains all libraries and dependencies need to run the
client. To test the client, move the jar file from the target directory to a working directory
(I use a directory called `C:\tools` which is included in my `PATH` environment variable) and issue the
following command:

```$xslt
java -jar C:\tools\sdkcoapclient-1.4.3.one-jar.jar --help
```

#### Determine Starfish Client Id and Client Secret
Your Starfish Client Id and Client Secret can be obtained from your Starfish Developer Program account.
Hopefully you captured these when you created your account. If not, you will need to create a new set
as Starfish does not store your client Id and Secret.
After you log into the developer program portal, select the "Account Settings" tab.
Towards the middle of thee page, locate and click on the "ClientID/Secret" tab.
Click on the "Create New ClientID/Secret" button. Make sure to write you Client Id and Client Secret down!

#### Determine Starfish Device Id
**TODO**

#### Determine Starfish Device Mac address
Your devices mac address is typically found on a sticker on the Nic board of your HDK. It will be a hexadecimal number. You will need your devices mac address when forming the host name of your device.

#### Setup CoAP Configuration Files
While you can use the client with command line options, it is usually easier to create a
configuration file that defines the CoAP activity you want to 
achieve. For example you might have
configuration files for the following activities:
- Start a new session
- Read data from an IoT sensor
- Send a command packet to an IoT control
- End a session

A configuration file contains a JSON representation of a subset of command line options.
Field names used in a configuration file are the same as used in command line options.

The following configuration JSON gives an example using almost all command line options:

```$xslt
{
  "proxyHost": "api.coap-staging.developer.ssni.com",
  "proxyPort": "5683",
  "clientPort": "6000",
  "observe": "true",
  "non": "",
  "maxNotifications": "5",
  "method" : "GET",
  "timeout": "900",
  "clientId": "<clientId>",
  "clientSecret": "<clientSecret>",
  "devices": [
    {
      "deviceId": "<deviceId>",
      "deviceHost": "SSN<macAddress>.SG.YEL01.SSN.SSNSGS.NET",
      "devicePort": "4849",
      "devicePath": "/snsr/arduino/temp",
      "deviceQuery": "sens"
    }
  ]
}
```

The following section describes the configuration/command line options available to you.

#### Configuration Options
All command line configuration options are described in this section.

| Option Name | Description |
| --- | --- |
| proxyHost | The IP address or hostname of a CoAP proxy. When using the client to communicate with Silverspring IoT devices you must always use the developer program CoAP proxy.
| proxyPort | The port number of the CoAP proxy.
| clientPort | The UDP listening port used by the client. Default is 6000.
| method | The CoAP method. Default is GET.
| observe | Used in conjunction with the GET method. Include to establish an observe on a CoAP resource. Default is false.
| non | Indicates that the CoAP request message is sent as non-confirmable (NON). Default is CON.
| maxNotifications | Sets a limit on how many observe notifications are read by the client. Default is 1. Note that after the client shuts down after maxNotifications have been received, the CoAP resources observe is not cancelled.
| timeout | The time in seconds allowed for a response from a CoAP activity. If an observe has been established the client will shutdown on the timeout even if maxNotifications have not been received. It is up to the user to set the timeout value accordingly. Default is 60 seconds.
| clientId | Your Starfish account client id.
| clientSecret | Your Starfish account client secret.

**Note**: If no proxy is used, the proxyHost and proxyPort options must not be present in the
configuration.

**Note**: The clientId and clientSecret are used to obtain an authorization token for use when
establishing a session with the developer program gateway and when sending notification
responses (when observing a resource) to the Starfish Data Platform.

##### Device Related Configuration

| Option Name | Description |
| ----- | ----- |
| deviceId | Starfish device Id for the Silver Spring IoT device (HDK). Required when posting observation responses to the Starfish Data Platform. |
| deviceHost | Host name for your Silver Spring IoT device. |
| devicePort | UDP port of your devices CoAP server URI. Default is 5683. |
| devicePath | Path part of your devices CoAP server resource URI. Default is "/". |
| deviceQuery | Query part of your devices CoAP server resource URI (the stuff that follows the ?). |
| devicePayload | Hex encoded string to be used as a payload for PUT/POST. |

**Note**: The host name will typically have this form: SSN+**mac-address**+.+**network-domain**.
An example host name follows for the Starfish network domain follows:
```
SSN00135005003cb8e1.SG.YEL01.SSN.SSNSGS.NET
```

**Note**: network-domain for the developer program is: SG.YEL01.SSN.SSNSGS.NET
#### Usage Patterns
A usage pattern is a sequence of invoking the CoAP client multiple times with different
configurations. These patterns can be captured in a shell script allowing you to build
complex functionality.

A typical pattern looks like this:
- Get a gateway session
- One or more CoAP commands (such as a GET on a device)
- Delete the gateway session

Configuration files for getting a session, deleting a session, and doing simple CoAP
activities on a device are included in the source git repo.

You will find the following sample configurations:

| File Name | Description |
| --- | --- |
| getsession_token.json | Get new session based on client-id and client-secret |
| deletesession.json | Delete an active session |
| get_hdk_ping.json | CoAP Get on the device root - makes a simple ping to insure device connectivity |
| get_hdk_sensor.json | CoAP Get on device temp sensor - returns one data sample |
| get_hdk_observe.json | CoAP Observe on device temp sensor - returns multiple notifications |
