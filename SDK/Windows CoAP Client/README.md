#Silver Spring Networks Developer Repository
 
Milli Developer Kit Tool For CoAP Commands Sent To Starfish Head End

Reference implementation using Core CoAPSharp Library http://idaax.com/coapsharp/

Intent:

The UI provides support for operations GET, PUT, and DELETE.  
This application doesn’t support Observe which is a specific implementation of GET that includes 
OBSERVE CoAP message header option.  
The purpose of the UI is not to implement all features of CoAP, but rather to demonstrate how to
communicate with devices registered to the Starfish network with CoAP communications proxied through Starfish head end gateway.

How To:

Installation - The included Windows Installer Project creates an msi installer.
Installs to Itron/Starfish directory as X86 application.  Desktop shortcut will also be created.


Launch Application  - Accept License and you will be required to enter a Client ID, Client Secret as 
established in the Developer Portal.  The default Solution will be “sandbox”.

Your Milli devices will be loaded and you can do CoAP get requests for these objects.