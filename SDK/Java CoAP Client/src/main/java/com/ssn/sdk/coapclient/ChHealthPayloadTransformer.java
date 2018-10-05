package com.ssn.sdk.coapclient;

import org.json.JSONArray;
import org.json.JSONObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.StringTokenizer;
import java.util.TimeZone;

/**
 * Methane (CH) Sensor Health Payload Transformer. Methane device health payload in, Observations JSON out.
 *
 * @author Bob Luben
 */
public class ChHealthPayloadTransformer
{
    private Logger log = LoggerFactory.getLogger(this.getClass().getName());


    public String buildPayload(String observation)
    {
        // Need a timestamp. As the observation does not include a ts, use current client time (UTC).
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'");
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
        Date dt = new Date();
        String timestamp = sdf.format(dt);
        log.debug("timestamp: {}", timestamp);

        // Start JSON object with timestamp
        JSONObject joObservation = new JSONObject();
        joObservation.put("timestamp", timestamp);

        // Process as a stream of tokens
        StringTokenizer oTokens = new StringTokenizer(observation, ",");
        String aToken = oTokens.nextElement().toString();

        // Token will be one of the following four elements
        // Gas level is the only pair that starts with an integer.
        if (aToken.matches("\\d+"))
        {
            Integer hoursUp = new Integer(aToken);
            joObservation.put("hoursSincePowerUp", hoursUp);
        }

        oTokens.nextElement();
        aToken = oTokens.nextElement().toString();
        if (aToken.equalsIgnoreCase("OK"))
        {
            joObservation.put("deviceHealth", "OK");
        }
        else
        {
            joObservation.put("deviceHealth", "NG");
        }

        oTokens.nextElement();
        aToken = oTokens.nextElement().toString();
        if (aToken.equalsIgnoreCase("OK"))
        {
            joObservation.put("batteryHealth", "OK");
        }
        else
        {
            joObservation.put("batteryHealth", "LB");
        }

        // Build payload JSON
        JSONArray joaObservations = new JSONArray();
        joaObservations.put(joObservation);

        JSONObject jo = new JSONObject();
        jo.put("observations", joaObservations);

        String json = jo.toString();
        log.info("Starfish payload: {}", json);
        return json;
    }
}
