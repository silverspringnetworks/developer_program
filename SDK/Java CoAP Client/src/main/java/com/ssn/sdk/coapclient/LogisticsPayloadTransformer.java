package com.ssn.sdk.coapclient;

import org.json.JSONArray;
import org.json.JSONObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.Base64;

/**
 * Logistics Payload Transformer. TLV device payload in, Logistics Observations JSON out.
 *
 * @author Jose Olcese
 */
public class LogisticsPayloadTransformer
{
    private Logger log = LoggerFactory.getLogger(this.getClass().getName());


    public String buildPayload(byte[] observation, String palletMacAddress, String apMacAddress)
    {

        byte[] encodedBytes = Base64.getEncoder().encode(observation);

        // {
        //     "palletteMacAddress" : "00135005FF0662E1",
        //     "apMacAddress" : "0013500300588AFD",
        //     "sensorData": <base64 encoded sensor data TLV payload (see above)>
        // }

        // Build payload
        JSONObject jo = new JSONObject();
        jo.put("palletMacAddress", palletMacAddress);
        jo.put("apMacAddress", apMacAddress);
        jo.put("sensorData", new String(encodedBytes));

        String json = jo.toString();
        log.info("Logistics payload: {}", json);

        return json;
    }
}
