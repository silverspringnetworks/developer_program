/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient.callback;

import co.nstant.in.cbor.CborException;
import com.ssn.sdk.coapclient.config.OptionsArgumentsWrapper;
import com.ssn.sdk.coapclient.sdp.StarfishClient;
import com.ssn.sdk.coapclient.util.PayloadUtilities;
import de.uzl.itm.ncoap.message.CoapMessage;
import de.uzl.itm.ncoap.message.CoapResponse;
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
public class SdkObservationCallback extends SdkCallback
{
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

        PayloadUtilities pu = new PayloadUtilities();
        String payloadAsHex = pu.bytesToHexString(payloadAsByteArray);
        log.info("***Payload As Hex: <{}>", payloadAsHex);


        // Check for error on the request. If so, no need to transform the payload.
        if (coapResponse.isErrorResponse())
        {
            log.error("***CoAP Response Error: {}", coapResponse.getMessageCode());
            return;
        }

        if (payloadAsStr.length() > 0)
        {
            // Select the payload trasnformer to use based on the resource path.
            if (arguments.getDevicePath().equalsIgnoreCase("/snsr/arduino/temp"))
            {
                StarfishClient sfc = new StarfishClient(arguments.getClientId(), arguments.getClientSecret(), arguments.getDeviceId(), arguments.isTestEnv());
                sfc.sendObservation(payloadAsByteArray, "com.ssn.sdk.coapclient.payload.TempPayloadTransformer");
            }
            // CH4 methane paylod (New Cosmos rl78 device)
            // Updated to support the CBOR wrapper format.
            else if (arguments.getDevicePath().equalsIgnoreCase("/snsr/rl78/methane"))
            {
                // Remove the CBOR wrapper
                byte[] ch4PayloadAsByteArray;
                try
                {
                    ch4PayloadAsByteArray = pu.stripOffCborWrapperByte(payloadAsByteArray);
                }
                catch (CborException excptn)
                {
                    log.error("*** Error removing CBOR wrapper from r178 payload: <{}>", excptn.getMessage());
                    return;
                }
                String ch4PayloadAsStr = new String(ch4PayloadAsByteArray, CoapMessage.CHARSET);
                log.info("*** CH4 Inner payload As String: <{}>", ch4PayloadAsStr);

                // Pass the rl78 data on as a string
                StarfishClient sfc = new StarfishClient(arguments.getClientId(), arguments.getClientSecret(), arguments.getDeviceId(), arguments.isTestEnv());
                sfc.sendObservation(ch4PayloadAsStr, "com.ssn.sdk.coapclient.payload.ChAlertPayloadTransformer");
            }
            else if (arguments.getDevicePath().equalsIgnoreCase("/snsr/logis/sens") || arguments.getDevicePath().equalsIgnoreCase("/snsr/logis/log"))
            {
                StarfishClient sfc = new StarfishClient(arguments.getClientId(), arguments.getClientSecret(), arguments.getDeviceId(), arguments.isTestEnv());
                log.info("Sending observation to Logistics");
                sfc.sendObservation(payloadAsByteArray, "com.ssn.sdk.coapclient.payload.LogisticsPayloadTransformer");
            }
            else
            {
                log.info("Got data from unknown sensor: {}. Ignoring data!", arguments.getDevicePath());
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
}
