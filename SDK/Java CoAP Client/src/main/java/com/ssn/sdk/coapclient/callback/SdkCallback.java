/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient.callback;

import com.ssn.sdk.coapclient.StarfishClient;
import com.ssn.sdk.coapclient.config.OptionsArgumentsWrapper;
import de.uzl.itm.ncoap.application.client.ClientCallback;
import de.uzl.itm.ncoap.message.CoapResponse;
import de.uzl.itm.ncoap.message.CoapMessage;
import org.jboss.netty.buffer.ChannelBuffer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * A  simple implementation of ClientCallback with little client side logic.
 *
 * @author bluben
 */
public class SdkCallback extends ClientCallback
{
    private Logger log = LoggerFactory.getLogger(this.getClass().getName());

    private AtomicBoolean responseReceived;
    private AtomicInteger transmissionCounter;
    private AtomicBoolean timedOut;
    private OptionsArgumentsWrapper arguments;


    // Default constructor
    public SdkCallback()
    {
        this.responseReceived = new AtomicBoolean(false);
        this.transmissionCounter = new AtomicInteger(0);
        this.timedOut = new AtomicBoolean(false);

        this.arguments = null;
    }

    // Standard constructor
    public SdkCallback(OptionsArgumentsWrapper arguments)
    {
        this.responseReceived = new AtomicBoolean(false);
        this.transmissionCounter = new AtomicInteger(0);
        this.timedOut = new AtomicBoolean(false);

        this.arguments = arguments;
    }

    /**
     * Increases the reponse counter by 1, i.e. {@link #getResponseCount()} will return a higher value after
     * invocation of this method.
     *
     * @param coapResponse the response message
     */
    @Override
    public void processCoapResponse(CoapResponse coapResponse)
    {
        responseReceived.set(true);
        log.info("Received #: CoAP: {}", coapResponse);

        ChannelBuffer response = coapResponse.getContent();
        String payloadAsStr = response.toString(CoapMessage.CHARSET);
        log.info("***Payload As String: <{}>", payloadAsStr);

        String payloadAsHex = this.bytesToHexString(response.array());
        log.info("***Payload As Hex: <{}>", payloadAsHex);

        //NOTE: Special handling for New Cosmos. Push ODR response to Starfish.
        if (payloadAsStr.length() > 0) {
            StarfishClient starfishClient = new StarfishClient(arguments.getClientId(), arguments.getClientSecret(), arguments.getDeviceId(), arguments.isTestEnv());

            // Select the payload trasnformer to use based on the resource path.
            if (arguments.getDevicePath().equalsIgnoreCase("/sensor/arduino/temp"))
            {
                log.info("Sending observation to Starfish");
                starfishClient.sendObservation(payloadAsStr, "com.ssn.sdk.coapclient.TempPayloadTransformer");
            }
            if (arguments.getDevicePath().equalsIgnoreCase("/sensor/rl78/methane"))
            {
                log.info("Sending observation to Starfish");
                starfishClient.sendObservation(payloadAsStr, "com.ssn.sdk.coapclient.ChAlertPayloadTransformer");
            }
        }
    }

    /**
     * Returns the number of responses received
     * @return the number of responses received
     */
    public int getResponseCount()
    {
        return this.responseReceived.get() ? 1 : 0;
    }


    @Override
    public void processRetransmission()
    {
        int value = transmissionCounter.incrementAndGet();
        log.info("Retransmission #{}", value);
    }

    @Override
    public void processMiscellaneousError(final String description)
    {
        log.error("ERROR: " + description);
    }

    @Override
    public void processResponseBlockReceived(long receivedLength, long expectedLength)
    {
        log.info("Received " + receivedLength + "/" + expectedLength + " bytes.");
    }


    @Override
    public void processTransmissionTimeout()
    {
        log.info("CoAPRequest time out!");
        timedOut.set(true);
    }


    public boolean isTimedOut()
    {
        return timedOut.get();
    }


    private String bytesToHexString(byte[] bytes){
        StringBuilder sb = new StringBuilder();
        for(byte b : bytes){
            sb.append(String.format("%02x", b&0xff));
        }
        return sb.toString();
    }
}
