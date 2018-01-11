package com.ssn.sdk.coapclient;

import org.json.JSONArray;
import org.json.JSONObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;

/**
 * Tempurature Sensor Payload Transformer. HDK device payload in, Observations JSON out.
 *
 * @author Bob Luben
 */
public class TempPayloadTransformer
{
    private Logger log = LoggerFactory.getLogger(this.getClass().getName());


    public String buildPayload(String observation)
    {
        String[] parts = observation.split(",", 3);

        Long ts = new Long(parts[0]);
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'");
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
        Date dt = new Date(ts);
        String timestamp = sdf.format(dt);
        log.debug("timestamp: {}", timestamp);

        // Convert the observation
        JSONObject joObservation = new JSONObject();
        joObservation.put("timestamp", timestamp);
        joObservation.put("temperature", parts[1]);

        JSONArray joaObservations = new JSONArray();
        joaObservations.put(joObservation);

        // Build payload
        JSONObject jo = new JSONObject();
        jo.put("observations", joaObservations);

        String json = jo.toString();
        log.info("Starfish payload: {}", json);

        return json;
    }
}
