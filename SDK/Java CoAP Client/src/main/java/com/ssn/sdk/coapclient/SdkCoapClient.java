/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient;

import com.ssn.sdk.coapclient.callback.SdkCallback;
import com.ssn.sdk.coapclient.callback.SdkObservationCallback;
import com.ssn.sdk.coapclient.config.OptionsArgumentsWrapper;
import com.ssn.sdk.coapclient.config.LoggingConfiguration;
import de.uzl.itm.ncoap.application.client.CoapClient;
import de.uzl.itm.ncoap.communication.blockwise.BlockSize;
import de.uzl.itm.ncoap.message.*;
import de.uzl.itm.ncoap.message.options.ContentFormat;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.net.*;


/**
 * SDK CoAP Client. Uses the nCoAP library. Loosely based on the simple CoAP client sample provided with nCoAP.
 *
 * @author Bob Luben
 */
public class SdkCoapClient extends CoapClient
{
    private Logger log = LoggerFactory.getLogger(this.getClass().getName());

    private OptionsArgumentsWrapper arguments;
    private SdkCallback callback;

    /**
     * Creates a new instance of {@link SdkCoapClient}.
     *
     * @param arguments an instance of {@link OptionsArgumentsWrapper} containing the given command line
     *                  parameters (use --help as parameter to print the supported parameters)
     */
    public SdkCoapClient(OptionsArgumentsWrapper arguments)
    {
        super("SDKCoAPClient", arguments.getClientPort());
        this.arguments = arguments;
    }

    public void sendCoapRequest() throws URISyntaxException, UnknownHostException {

        int messageCode = MessageCode.GET;
        if (arguments.getMethod().equalsIgnoreCase("GET"))
        {
            messageCode= MessageCode.GET;
        }
        if (arguments.getMethod().equalsIgnoreCase("DELETE"))
        {
            messageCode= MessageCode.DELETE;
        }
        if (arguments.getMethod().equalsIgnoreCase("POST"))
        {
            messageCode= MessageCode.POST;
        }
        if (arguments.getMethod().equalsIgnoreCase("PUT"))
        {
            messageCode= MessageCode.PUT;
        }

        // determine the URI of the resource to be requested
        String host = arguments.getDeviceHost();
        int port = arguments.getDevicePort();
        String path = arguments.getDevicePath();
        String query = arguments.getDeviceQuery();
        URI resourceURI = new URI("coap", null, host, port, path, query, null);

        // create the request
        boolean useProxy = arguments.getProxyAddress() != null;
        int messageType = arguments.isNon() ? MessageType.NON : MessageType.CON;
        CoapRequest coapRequest = new CoapRequest(messageType, messageCode, resourceURI, useProxy);

        if (messageCode == MessageCode.GET)
        {
            coapRequest.setPreferredBlock2Size(BlockSize.SIZE_128);
        }
        coapRequest.setEndpointID1();

        // observe resource or not?
        if (arguments.isObserve())
        {
            coapRequest.setObserve(0);
        }

        // If it's a session call
        // TODO: Generalize
        if (messageCode == MessageCode.POST || messageCode == MessageCode.PUT)
        {
            if (path.equals("/sessions") && (arguments.getClientId() != null && arguments.getClientId().length() > 0))
            {
                TokenClient tc = new TokenClient();
                String token;
                try
                {
                    token = tc.getApiToken(arguments.getClientId(), arguments.getClientSecret());
                } catch (Exception excpn)
                {
                    log.error("Aborting: Failed to acquire API token: {}", excpn.getMessage());
                    return;
                }
                byte[] payload = token.getBytes();
                coapRequest.setContent(payload, ContentFormat.TEXT_PLAIN_UTF8);
            }
        }

        // determine recipient (proxy or CoAP resource host)
        InetSocketAddress remoteSocket;
        if (useProxy) {
            InetAddress proxyAddress = InetAddress.getByName(arguments.getProxyAddress());
            int proxyPort = arguments.getProxyPort();
            remoteSocket = new InetSocketAddress(proxyAddress, proxyPort);
        } else {
            InetAddress serverAddress = InetAddress.getByName(arguments.getDeviceHost());
            int serverPort = arguments.getDevicePort();
            remoteSocket = new InetSocketAddress(serverAddress, serverPort);
        }

        // define the client callback
        if (arguments.isObserve()) {
            callback = new SdkObservationCallback(arguments);
        } else {
            callback = new SdkCallback();
        }

        //Send the CoAP request
        this.sendCoapRequest(coapRequest, remoteSocket, callback);
    }


    public void waitAndShutdown() throws InterruptedException {
        long startTime = System.currentTimeMillis();

        while (!isShutdownCriterionSatisfied(startTime)) {
            Thread.sleep(100);
        }

        //Wait for another 10 seconds to answer the next update notification with a RST to stop the observation
        if (arguments.isObserve()) {
            Thread.sleep(10000);
        }

        this.shutdown();
    }


    public boolean isShutdownCriterionSatisfied(long startTime) {

        //Check if maximum duration was reached
        if ((System.currentTimeMillis() - startTime) / 1000 > arguments.getTimeout()) {
            return true;
        }

        //Check message count or transmission timeout
        int responseCount = callback.getResponseCount();
        int awaitedResponses = arguments.isObserve() ? arguments.getMaxNotifications() : 1;

        return !(responseCount < awaitedResponses && !callback.isTimedOut());
    }


    /**
     * Creates an instance of {@link SdkCoapClient} and starts to do whatever was defined using the command line
     * parameters. Use <code>--help</code> as command line option to print a list of available parameters.
     *
     * @param args command line options
     * @throws Exception
     */
    public static void main(String[] args) throws Exception
    {
        // Configure logging
        LoggingConfiguration.configureLogging();

        // Process command line options and any config file
        OptionsArgumentsWrapper arguments = new OptionsArgumentsWrapper(args);
        arguments.mergeConfigFileOptions();

        // Start the client
        SdkCoapClient client = new SdkCoapClient(arguments);

        // Send the request
        client.sendCoapRequest();

        // wait for shutdown criterion and shutdown...
        client.waitAndShutdown();
    }
}
