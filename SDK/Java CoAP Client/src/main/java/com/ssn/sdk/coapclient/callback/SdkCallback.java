/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient.callback;

import co.nstant.in.cbor.CborException;
import com.ssn.sdk.coapclient.sdp.StarfishClient;
import com.ssn.sdk.coapclient.config.OptionsArgumentsWrapper;
import com.ssn.sdk.coapclient.util.PayloadUtilities;
import de.uzl.itm.ncoap.application.client.ClientCallback;
import de.uzl.itm.ncoap.message.CoapResponse;
import de.uzl.itm.ncoap.message.CoapMessage;
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

        // Select the payload transformer to use based on the resource path (i.e. a sensor)
        if (payloadAsStr.length() > 0)
        {
            if (arguments.getDevicePath().equalsIgnoreCase("/snsr/arduino/temp"))
            {
                StarfishClient sfc = new StarfishClient(arguments.getClientId(), arguments.getClientSecret(), arguments.getDeviceId());
                sfc.sendObservation(payloadAsByteArray, "com.ssn.sdk.coapclient.payload.TempPayloadTransformer");
            }
            else if (arguments.getDevicePath().equalsIgnoreCase("/snsr/rl78/methane"))
            {
                // Remove the CBOR wrapper
                String innerPayloadAsStr;
                try
                {
                    innerPayloadAsStr = pu.stripOffCborWrapperString(payloadAsByteArray);
                }
                catch (CborException excptn)
                {
                    log.error("*** Error removing CBOR wrapper from r178 payload: <{}>", excptn.getMessage());
                    return;
                }
                log.info("*** CH4 Inner payload As String: <{}>", innerPayloadAsStr);

                StarfishClient sfc = new StarfishClient(arguments.getClientId(), arguments.getClientSecret(), arguments.getDeviceId());
                sfc.sendObservation(innerPayloadAsStr, "com.ssn.sdk.coapclient.payload.ChAlertPayloadTransformer");
            }
            else if (arguments.getDevicePath().equalsIgnoreCase("/snsr/logis/sens") || arguments.getDevicePath().equalsIgnoreCase("/snsr/logis/log"))
            {
                StarfishClient sfc = new StarfishClient(arguments.getClientId(), arguments.getClientSecret(), arguments.getDeviceId());
                sfc.sendObservation(payloadAsByteArray, "com.ssn.sdk.coapclient.payload.LogisticsPayloadTransformer");
            }
            else
            {
                log.info("Got data from unknown sensor: {}. Ignoring data!", arguments.getDevicePath());
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
}
