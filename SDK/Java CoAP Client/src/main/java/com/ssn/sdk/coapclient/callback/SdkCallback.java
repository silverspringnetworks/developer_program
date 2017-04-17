/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient.callback;

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


    public SdkCallback()
    {
        this.responseReceived = new AtomicBoolean(false);
        this.transmissionCounter = new AtomicInteger(0);
        this.timedOut = new AtomicBoolean(false);
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
