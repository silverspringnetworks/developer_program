/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient.callback;

import com.ssn.sdk.coapclient.config.OptionsArgumentsWrapper;
import com.ssn.sdk.coapclient.StarfishClient;
import de.uzl.itm.ncoap.message.CoapMessage;
import de.uzl.itm.ncoap.message.CoapResponse;
import org.jboss.netty.buffer.ChannelBuffer;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.lang.Override;
import java.util.concurrent.atomic.AtomicInteger;

/**
 * A simple callback for handling observations. After the specified number of notifications have arrived it will
 * cancel the observation.
 *
 * @author bluben
 */
public class SdkObservationCallback extends SdkCallback {

    private Logger log = LoggerFactory.getLogger(this.getClass().getName());

    private AtomicInteger responseCounter;
    private OptionsArgumentsWrapper arguments;

    /**
     * Creates a new instance of {@link SdkObservationCallback}
     *
     * @param arguments the number of update notifications to be received until the
     *                                            observation is automatically stopped
     */
    public SdkObservationCallback(OptionsArgumentsWrapper arguments)
    {
        this.responseCounter = new AtomicInteger(0);
        this.arguments = arguments;
    }


    /**
     * Process a notification .Increases the response counter by 1.
     *
     * @param coapResponse the notification message
     */
    @Override
    public void processCoapResponse(CoapResponse coapResponse)
    {
        int value = responseCounter.incrementAndGet();
        log.info("Notification #{}: CoAP: {}", value, coapResponse);

        ChannelBuffer response = coapResponse.getContent();
        String payloadAsStr = response.toString(CoapMessage.CHARSET);
        log.info("***Payload As String: <{}>", payloadAsStr);

        String payloadAsHex = this.bytesToHexString(response.array());
        log.info("***Payload As Hex: <{}>", payloadAsHex);


        if (payloadAsStr.length() > 0) {
            log.info("Sending observation to Starfish");
            StarfishClient starfishClient = new StarfishClient(arguments.getClientId(), arguments.getClientSecret(), arguments.getDeviceId(), arguments.isTestEnv());
            starfishClient.sendObservation(payloadAsStr);
        }
    }


    @Override
    public int getResponseCount()
    {
        return responseCounter.intValue();
    }


    @Override
    public boolean continueObservation()
    {
        log.info("continueObservation: {}", getResponseCount());
        boolean result = getResponseCount() < arguments.getMaxNotifications();
        log.info("Received {}/{} responses (continue observation: {})", result);
        return result;
    }


    private String bytesToHexString(byte[] bytes){
        StringBuilder sb = new StringBuilder();
        for(byte b : bytes){
            sb.append(String.format("%02x", b&0xff));
        }
        return sb.toString();
    }
}
