package com.itron.enablement.sdk.mqtt.util;

import co.nstant.in.cbor.CborDecoder;
import co.nstant.in.cbor.CborException;
import co.nstant.in.cbor.model.DataItem;
import co.nstant.in.cbor.model.ByteString;
import co.nstant.in.cbor.model.UnicodeString;
import co.nstant.in.cbor.model.UnsignedInteger;
import com.itron.enablement.sdk.mqtt.sdp.StarfishClient;
import com.itron.enablement.sdk.mqtt.sdp.TokenClient;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.io.ByteArrayInputStream;
import java.util.List;
import java.util.Scanner;

public class PayloadUtilities
{
    private static final Logger log = LogManager.getLogger(PayloadUtilities.class.getName());


    // Helper to convert byte array to hex string
    public String bytesToHexString(byte[] bytes)
    {
        StringBuilder sb = new StringBuilder();
        for(byte b : bytes)
        {
            sb.append(String.format("%02x", b&0xff));
        }
        return sb.toString();
    }


    // Helper that converts a InputStream to a String
    public String convertStreamToString(java.io.InputStream is)
    {
        Scanner s = new Scanner(is).useDelimiter("\\A");
        return s.hasNext() ? s.next() : "";
    }


    // Helper to send an observation to Starfish Data Platform
    //
    public boolean sendObservation2StarfishDataPlatform(String deviceId, String oneLnicObservation, MqttSdpConfiguration config)
    {
        try
        {
            if (deviceId != null)
            {
                String token;
                token = getStarfishToken(config);

                if (token == null)
                {
                    log.error("Can't send to SDP: Dropping message!");
                }
                StarfishClient sfc = new StarfishClient(config);
                sfc.sendObservations(deviceId, oneLnicObservation, token);
            }
        }
        catch (Exception excptn)
        {
            log.error("Exception in sendObservation2StarfishDataPlatform: {}", excptn.getMessage());
            log.info("Skipping send of observation to data platform!");
        }
        return false;
    }


    public String getStarfishToken(MqttSdpConfiguration config)
    {
        // Get a Starfish token
        TokenClient tc = new TokenClient(config);
        String token;
        try
        {
            token = tc.getApiToken();
        }
        catch (Exception excpn)
        {
            log.error("Failed to get Starfish token with Exception: {}", excpn.getMessage());
            return null;
        }
        if (token == null)
        {
            log.error("No token returned from TokenClient!");
            return null;
        }
        return token;
    }


    // Helper to strip off an outer CBOR wrapper. Case where "inner" payload is a byte string type.
    public byte[] stripOffCborWrapperByte(byte[] payload) throws CborException
    {
        ByteArrayInputStream bais = new ByteArrayInputStream(payload);
        List<DataItem> dataItems = new CborDecoder(bais).decode();
        co.nstant.in.cbor.model.Map payloadMap = (co.nstant.in.cbor.model.Map) dataItems.get(0);

        ByteString data = (ByteString) payloadMap.get(new UnsignedInteger(1));
        byte[] innerPayload = data.getBytes();

        return innerPayload;
    }


    // Helper to strip off an outer CBOR wrapper. Case where "inner" payload is text type.
    public String stripOffCborWrapperString(byte[] payload) throws CborException
    {
        ByteArrayInputStream bais = new ByteArrayInputStream(payload);
        List<DataItem> dataItems = new CborDecoder(bais).decode();
        co.nstant.in.cbor.model.Map payloadMap = (co.nstant.in.cbor.model.Map) dataItems.get(0);

        UnicodeString innerPayload = (UnicodeString) payloadMap.get(new UnsignedInteger(1));

        return innerPayload.getString();
    }
}
