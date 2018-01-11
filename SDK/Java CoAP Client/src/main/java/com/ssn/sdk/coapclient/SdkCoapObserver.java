/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient;

import com.ssn.sdk.coapclient.config.OptionsArgumentsWrapper;
import com.ssn.sdk.coapclient.config.LoggingConfiguration;
import org.kohsuke.args4j.CmdLineException;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.net.URISyntaxException;
import java.net.UnknownHostException;

/**
 * Not yest complete. Coming soon.
 *
 * @author Bob Luben
 */
public class SdkCoapObserver
{
    private Logger log = LoggerFactory.getLogger(this.getClass().getName());

    //private static String proxyHost = "74.121.23.204";
    private static String proxyHost = "10.59.204.57";
    private static String proxyPort = "5683";
    private static String deviceHost = "SSN00135005003cb8e1.TEAM-ATS-23X-INSTALL.ENG.SSNSGS.NET";
    private static String devicePort = "5683";
    private static String devicePath = "/snsr/arduino/temp";
    private static String deviceQuery = "sens";

    private static String sessionPath = "/sessions";
    private static String sessionQuery = "user=bluben&pass=2Doll@r$&security=none&sticky=true";

    private Integer maxReceiveCount = 1;
    private Integer maxTimeLimit = 90;
    private int clientObserverPort = 6000;

    private CoapDevice observerDevice = null;

    /**
     *  Initializes an instance of {@link SdkCoapObserver}.
     *
     */
    public SdkCoapObserver(int maxObservationCount, int maxTmeLimit, int observerPort )
    {
        maxReceiveCount = maxObservationCount;
        maxTimeLimit = maxTmeLimit;
        clientObserverPort = observerPort;
    }

    /**
     * Creates an instance of {@link SdkCoapObserver} takes the device description and sets an observe on that device.
     *
     * @throws Exception if something went terribly wrong
     */
    public static void main(String[] args) throws Exception
    {
        //Configure logging
        LoggingConfiguration.configureLogging();

        SdkCoapObserver observer = new SdkCoapObserver(20, 250, 6000);

        // Get device information (everything needed to hand an observe)
        CoapDevice deviceToObserve = observer.getCoapDevice();
        deviceToObserve.setMaxAge(300);

        // Observe the device
        for (int indx = 0; indx < 10; indx++)
        {
            // Get a gateway session
            boolean gotSession = observer.createSession(proxyHost, proxyPort, sessionPath, sessionQuery);

            if (gotSession)
            {
                int rcode = observer.observeDevice(deviceToObserve);
                if (rcode <= 0) {
                    break;
                }
                deviceToObserve.setMaxAge(300);

                // This can take a long time. Code is running in the SdkCoapClient. Delete session on return.
                observer.deleteSession(proxyHost, proxyPort, sessionPath);
            }
        }

        // This can take a long time. Code is running in the SdkCoapClient. Delete session on return.
        observer.deleteSession(proxyHost, proxyPort, sessionPath);
    }

    private int observeDevice(CoapDevice deviceToObserve) throws InterruptedException
    {
        String[] directives = {"--observing", "--proxyAddress", proxyHost, "--proxyPort",
                proxyPort, "--host", deviceHost, "--port", devicePort,
                "--path", devicePath, "--query", deviceQuery,
                "--method", "GET",
                "--maxUpdates", maxReceiveCount.toString(), "--duration", maxTimeLimit.toString()};

        log.info("Start observations on a devices resource");
        OptionsArgumentsWrapper clientOptions = parseOptions(directives);

        // Get a CoAP client
        SdkCoapClient client = new SdkCoapClient(clientOptions);

        // Send the request
        if (!sendCoapRequest(client))
        {
            return -1;
        }

        // Check for max_age expiration
        long startTime = System.currentTimeMillis();
        while (!client.isShutdownCriterionSatisfied(startTime))
        {
            Thread.sleep(1000);
            deviceToObserve.setMaxAge((deviceToObserve.getMaxAge()-1));
            if (deviceToObserve.getMaxAge() <= 0)
            {
                log.warn("The max_age has expired. Shutting down observations");
                Thread.sleep(5000);
                client.shutdown();
                return 1;
            }
        }

        // Complete observe sequence
        log.info("Completed observations on a devices resource");
        client.shutdown();
        return 0;
    }

    private boolean createSession(String proxyHost, String proxyPort, String sessionPath, String sessionQuery)
    {
        String[] directives = {"--host", proxyHost, "--port", proxyPort, "--path", sessionPath, "--query", sessionQuery, "--method", "POST" };

        log.info("Create CoAP Gateway Session");
        OptionsArgumentsWrapper clientOptions = parseOptions(directives);

        // Get a CoAP client
        SdkCoapClient client = new SdkCoapClient(clientOptions);

        // Send the request
        if (!sendCoapRequest(client))
        {
            return false;
        }

        // wait for shutdown criterion and shutdown...
        try
        {
            client.waitAndShutdown();
        }
        catch(InterruptedException excptn)
        {
            log.error("CoAP request interrupted: {}", excptn.getMessage());
            return false;
        }

        log.info("Got CoAP Gateway Session");
        return true;
    }

    private boolean deleteSession(String proxyHost, String proxyPort, String sessionPath)
    {
        String[] directives = {"--host", proxyHost, "--port", proxyPort, "--path", sessionPath, "--method", "DELETE" };

        log.info("Delete CoAP Gateway Session");
        OptionsArgumentsWrapper clientOptions = parseOptions(directives);

        // Get a CoAP client
        SdkCoapClient client = new SdkCoapClient(clientOptions);

        // Send the request
        if (!sendCoapRequest(client))
        {
            return false;
        }

        // wait for shutdown criterion and shutdown...
        try
        {
            client.waitAndShutdown();
        }
        catch(InterruptedException excptn)
        {
            log.error("CoAP request interrupted: {}", excptn.getMessage());
            return false;
        }

        log.info("Deleted CoAP Gateway Session");
        return true;
    }

    private boolean sendCoapRequest(SdkCoapClient client)
    {
        // Send the request
        try
        {
            client.sendCoapRequest();
        }
        catch(URISyntaxException excptn)
        {
            log.error("Failed to send CoAP: Bad URI Syntax: {}", excptn.getMessage());
            return false;
        }
        catch(UnknownHostException excptn)
        {
            log.error("Failed to parse CoAP Client options: {}", excptn.getMessage());
            return false;
        }
        return true;
    }

    private OptionsArgumentsWrapper parseOptions(String[] directives)
    {
        OptionsArgumentsWrapper clientOptions = null;
        try
        {
            clientOptions = new OptionsArgumentsWrapper(directives);
        }
        catch(CmdLineException excptn)
        {
            log.error("Failed to parse CoAP Client options: {}", excptn.getMessage());
            throw new IllegalArgumentException();
        }
        return clientOptions;
    }

    private void initialize() throws Exception
    {
        //Configure logging
        LoggingConfiguration.configureLogging();
    }

    public CoapDevice getCoapDevice()
    {
        CoapDevice device = new CoapDevice();
        return device;
    }
}
