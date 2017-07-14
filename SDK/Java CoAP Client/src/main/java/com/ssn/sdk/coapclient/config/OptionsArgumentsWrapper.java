/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient.config;

import com.ssn.sdk.coapclient.SdkCoapClient;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

import de.uzl.itm.ncoap.message.MessageCode;
import org.json.*;
import org.kohsuke.args4j.CmdLineException;
import org.kohsuke.args4j.CmdLineParser;
import org.kohsuke.args4j.Option;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Wrapper around options and arguments for {@link SdkCoapClient}
 *
 * @author Bob Luben
 */
public class OptionsArgumentsWrapper
{
    private Logger log = LoggerFactory.getLogger(this.getClass().getName());

    @Option(name = "--conf",
            usage="Configuration file path ('')")
    private String confPath = null;

    @Option(name = "--clientPort",
            usage="Port client expects responses on (7000))")
    private int clientPort = 7000;

    @Option(name = "--method",
            usage="CoAP method (GET)")
    private String method = "GET";

    @Option(name = "--deviceHost",
            usage="IP address or CNAME of the device or server URI (localhost)")
    private String deviceHost = "localhost";

    @Option(name = "--devicePort",
            usage = "Port of the device or server URI (4849)")
    private int devicePort = 4849;

    @Option(name = "--devicePath",
            usage = "Sets the path of the target URI ('/')")
    private String devicePath = "/";

    @Option(name = "--deviceQuery",
            usage = "Sets the query of the target URI (default = null)")
    private String deviceQuery = null;

    @Option(name = "--deviceId",
            usage="Devices starfish device id (empty string)")
    private String deviceId = "";

    @Option(name = "--proxyHost",
            usage = "IP address or CNAME of a proxy the request is to be sent to (default = null)")
    private String proxyHost = null;

    @Option(name = "--proxyPort",
            usage = "Sets the port of a proxy the request is to be sent to (5683)")
    private int proxyPort = 5683;

    @Option(name = "--non",
            usage = "Empty argument that causes the request to be sent non-confirmable")
    private boolean non = false;

    @Option(name = "--timeout",
            usage = "Sets the maximum timeout (in seconds) before shutdown (default = 60)")
    private int timeout = 60;

    @Option(name = "--observe",
            usage = "Empty argument that causes specified resource to be observed")
    private boolean observe = false;

    @Option(name = "--maxNotifications",
            usage = "Sets the number of update notifications before shutdown (default = 1)")
    private int maxNotifications = 1;

    @Option(name = "--clientId",
            usage = "Sets the Starfish cleint Id ('')")
    private String clientId = "";

    @Option(name = "--clientSecret",
            usage = "Sets the Starfish cleint Secret ('')")
    private String clientSecret = "";

    @Option(name = "--test",
            usage = "Empty argument that sets endpoints for the TEST network")
    private boolean testenv = false;

    @Option(name = "--help",
            usage = "Prints option help")
    private boolean help = false;


    /**
     * Creates a new instance of {@link OptionsArgumentsWrapper}.
     *
     * @param args the array of command line options (usually the args parameter from
     *             <code>public static void main(String[] args)</code>
     *
     * @throws CmdLineException if some error occurred while reading the given command line arguments
     */
    public OptionsArgumentsWrapper(String[] args) throws CmdLineException
    {
        CmdLineParser parser = new CmdLineParser(this);

        try{
            parser.parseArgument(args);
        }
        catch(CmdLineException excpn) {
            System.err.println(excpn.getMessage());
            parser.printUsage(System.err);
            throw excpn;
        }

        if (this.isHelp())
        {
            parser.printUsage(System.out);
            System.exit(0);
        }
    }


    public void mergeConfigFileOptions()
    {
        if (this.getConfPath() == null || this.getConfPath().equals(""))
            return;

        String conf = null;
        try
        {
            conf = new String(Files.readAllBytes(Paths.get(this.getConfPath())));
        }
        catch (IOException excpn)
        {
            log.error("Failed to process conf file: {}", excpn.getMessage());
        }

        JSONObject jo = new JSONObject(conf);

        String soption = jo.optString("proxyHost");
        if (soption != null && !soption.equals(""))
        {
            proxyHost = soption;
            log.debug("Conf proxyHost: {}", soption);
        }
        long ioption = jo.optLong("proxyPort");
        if (ioption != 0)
        {
            proxyPort = (int)ioption;
            log.debug("Conf proxyPort: {}", ioption);
        }
        ioption = jo.optLong("clientPort");
        if (ioption != 0)
        {
            clientPort = (int)ioption;
            log.debug("Conf clientPort: {}", ioption);
        }
        soption = jo.optString("method");
        if (soption != null && !soption.equals(""))
        {
            method = soption;
            log.debug("Conf method: {}", soption);
        }
        boolean boption = jo.optBoolean("observe");
        if (boption)
        {
            observe = true;
            log.debug("Conf observe: {}", true);
        }
        ioption = jo.optLong("maxNotifications");
        if (ioption != 0)
        {
            maxNotifications = (int)ioption;
            log.debug("Conf maxNotifications: {}", ioption);
        }
        ioption = jo.optLong("timeout");
        if (ioption != 0) {
            timeout = (int) ioption;
            log.debug("Conf timeout: {}", ioption);
        }
        soption = jo.optString("clientId");
        if (soption != null && !soption.equals(""))
        {
            clientId = soption;
            log.debug("Conf clientId: {}", clientId);
        }
        soption = jo.optString("clientSecret");
        if (soption != null && !soption.equals(""))
        {
            clientSecret = soption;
            log.debug("Conf clientSecret: {}", clientSecret);
        }
        boption = jo.optBoolean("test");
        if (boption)
        {
            testenv = true;
            log.debug("Conf test: {}", true);
        }

        JSONArray darray = jo.getJSONArray("devices");
        if (darray.length() >= 0)
        {
            soption = darray.getJSONObject(0).optString("deviceId");
            if (soption != null && !soption.equals("")) {
                deviceId = soption;
                log.debug("Conf deviceId: {}", deviceId);
            }

            if (soption != null && !soption.equals("")) {
                soption = darray.getJSONObject(0).optString("deviceHost");
                deviceHost = soption;
                log.debug("Conf deviceHost: {}", deviceHost);
            }

            ioption = darray.getJSONObject(0).optLong("devicePort");
            if (ioption != 0)
            {
                devicePort = (int)ioption;
                log.debug("Conf devicePort: {}", devicePort);
            }

            soption = darray.getJSONObject(0).optString("devicePath");
            if (soption != null && !soption.equals("")) {
                devicePath = soption;
                log.debug("Conf devicePath: {}", devicePath);
            }

            soption = darray.getJSONObject(0).optString("deviceQuery");
            if (soption != null && !soption.equals("")) {
                deviceQuery = soption;
                log.debug("Conf deviceQuery: {}", deviceQuery);
            }
        }
    }

    /**
     * @return configuration file path  or <code>conf.json</code> if not set
     */
    public String getConfPath() {
        return confPath;
    }

    /**
     * @return the port of the client response port or <code>6000</code> if not set
     */
    public int getClientPort() {
        return clientPort;
    }

    /**
     * @return the CoAP method to be used with request or <code>GET</code> if not set
     */
    public String getMethod() {
        return method;
    }

    /**
     * @return the host of the URI the request is to be sent to or <code>localhost</code> if not set
     */
    public String getDeviceHost() {
        return deviceHost;
    }

    /**
     * @return the port of the URI the request is to be sent to or <code>5683</code> if not set
     */
    public int getDevicePort() {
        return devicePort;
    }

    /**
     * @return the path of the URI the request is to be sent to or <code>null</code> if not set
     */
    public String getDevicePath() {
        return devicePath;
    }

    /**
     * @return the query of the URI the request is to be sent to or <code>null</code> if not set
     */
    public String getDeviceQuery() {
        return deviceQuery;
    }

    /**
     * @return the starfish device id associated with the device <code>""</code> if not set
     */
    public String getDeviceId() {
        return deviceId;
    }

    /**
     * @return the address of the proxy the request is to be sent to or <code>null</code> if not set
     */
    public String getProxyAddress() {
        return proxyHost;
    }

    /**
     * @return the port of the proxy the request is to be sent to or <code>5683</code> if not set
     */
    public int getProxyPort() {
        return proxyPort;
    }

    /**
     * @return <code>true</code> if the request is to be sent non-confirmable or <code>false</code> otherwise
     */
    public boolean isNon() {
        return non;
    }

    /**
     * @return the number of seconds to wait before the client is shut down or <code>60</code> if not set
     */
    public int getTimeout() {
        return timeout;
    }

    /**
     * @return <code>true</code> if <code>--observe</code> was set as command line parameter
     * and <code>false</code> otherwise.
     */
    public boolean isObserve() {
        return observe;
    }

    /**
     * @return the maximum nukber of notifications to wait for before the client is shut down or <code>1</code> if not set
     */
    public int getMaxNotifications() {
        return maxNotifications;
    }

    /**
     * @return the Starfish client Id or <code>''</code> if not set
     */
    public String getClientId() {
        return clientId;
    }

    /**
     * @return the Starfish client Secret or <code>''</code> if not set
     */
    public String getClientSecret() {
        return clientSecret;
    }

    /**
     * @return <code>true</code> if --test was given as console parameter or <code>false</code> otherwise
     */
    public boolean isTestEnv() {
        return testenv;
    }

    /**
     * @return <code>true</code> if --help was given as console parameter or <code>false</code> otherwise
     */
    public boolean isHelp() {
        return help;
    }


    /**
     * @return <code>MessageCode</code> the http method equivalent specified for the message.
     */
    public int getRequestMessageCode()
    {
        int messageCode = MessageCode.GET;
        if (this.getMethod().equalsIgnoreCase("GET"))
        {
            messageCode= MessageCode.GET;
        }
        if (this.getMethod().equalsIgnoreCase("DELETE"))
        {
            messageCode= MessageCode.DELETE;
        }
        if (this.getMethod().equalsIgnoreCase("POST"))
        {
            messageCode= MessageCode.POST;
        }
        if (this.getMethod().equalsIgnoreCase("PUT"))
        {
            messageCode= MessageCode.PUT;
        }

        return messageCode;
    }
}
