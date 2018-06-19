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

        byte[] payloadAsByteArray = coapResponse.getContentAsByteArray();
        log.info("***Payload length: {}", payloadAsByteArray.length);

        String payloadAsStr = new String(payloadAsByteArray, CoapMessage.CHARSET);
        log.info("***Payload As String: <{}>", payloadAsStr);

        String payloadAsHex = this.bytesToHexString(payloadAsByteArray);
        log.info("***Payload As Hex: <{}>", payloadAsHex);

        if (payloadAsStr.length() > 0) {
    
            // Select the payload trasnformer to use based on the resource path.
            if (arguments.getDevicePath().equalsIgnoreCase("/snsr/arduino/temp"))
            {
                StarfishClient starfishClient = new StarfishClient(arguments.getClientId(), arguments.getClientSecret(), arguments.getDeviceId(), arguments.isTestEnv());
                log.info("Sending observation to Starfish");
                starfishClient.sendObservation(payloadAsStr, "com.ssn.sdk.coapclient.TempPayloadTransformer");
            }
            // CH4 methane paylod (New Cosmos rl78 device)
            // Updated to support the CBOR wrapper format.
            else if (arguments.getDevicePath().equalsIgnoreCase("/snsr/rl78/methane"))
            {
                // Remove the CBOR wrapper
                final int cborwrapperlength = 11;
                byte[] ch4PayloadAsByteArray = stripOffCborWrapper(payloadAsByteArray, cborwrapperlength);
                String ch4PayloadAsStr = new String(ch4PayloadAsByteArray, CoapMessage.CHARSET);
                log.info("*** CH4 Inner payload As String: <{}>", ch4PayloadAsStr);

                // Pass the rl78 data on as a string
                StarfishClient starfishClient = new StarfishClient(arguments.getClientId(), arguments.getClientSecret(), arguments.getDeviceId(), arguments.isTestEnv());
                log.info("Sending observation to Starfish");
                starfishClient.sendObservation(ch4PayloadAsStr, "com.ssn.sdk.coapclient.ChAlertPayloadTransformer");
            }
            else if (arguments.getDevicePath().equalsIgnoreCase("/snsr/logis/sens") || arguments.getDevicePath().equalsIgnoreCase("/snsr/logis/log"))
            {
                StarfishClient logisticsClient = new StarfishClient(arguments.getClientId(), arguments.getClientSecret(), arguments.getDeviceId(), arguments.isTestEnv(), true, arguments.getApMacAddress(), arguments.getDeviceHost().substring(3,19));
                log.info("Sending observation to Logistics");
                logisticsClient.sendObservation(payloadAsByteArray, "com.ssn.sdk.coapclient.LogisticsPayloadTransformer");
            }
            else
            {
                log.error("Can't send observation. Unknown path: {}", arguments.getDevicePath());
            }
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


    // Helper to strip off an outer CBOR wrapper
    public byte[] stripOffCborWrapper(byte[] payload, int wrapperlength)
    {
        int innerLength = payload.length - wrapperlength;
        byte[] innerPayload = new byte[innerLength];

        // This is brain dead simple. We just skip the first 11 bytes.
        // Long term we need to use a CBOR parser.
        for (int outindx=wrapperlength, inindx=0; outindx < payload.length ; outindx++,inindx++)
        {
            innerPayload[inindx] = payload[outindx];
        }

        return innerPayload;
    }
}
