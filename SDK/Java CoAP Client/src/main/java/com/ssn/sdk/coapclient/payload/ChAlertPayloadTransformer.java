package com.ssn.sdk.coapclient.payload;

import org.json.JSONArray;
import org.json.JSONObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.StringTokenizer;
import java.util.TimeZone;

/**
 * Methane (CH) Sensor Alert Payload Transformer. Methane device payload in, Observations JSON out.
 *
 * @author Bob Luben
 */
public class ChAlertPayloadTransformer
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
        while (oTokens.hasMoreElements())
        {
            String aToken = oTokens.nextElement().toString();

            // Token will be one of the following four elements
            // Gas level is the only pair that starts with an integer.
            if (aToken.matches("\\d+"))
            {
                Integer aValue = new Integer(aToken);
                String healthOrAlertToken = oTokens.nextElement().toString();
                if (healthOrAlertToken.equalsIgnoreCase("h"))
                {
                    // Health - expect specific order
                    joObservation.put("hoursSincePowerUp", aValue);
                    if (oTokens.hasMoreElements())
                    {
                        aToken = oTokens.nextElement().toString();
                        if (aToken.equalsIgnoreCase("NG"))
                        {
                            joObservation.put("deviceHealth", "NG");
                        }
                        else
                        {
                            joObservation.put("deviceHealth", "OK");
                        }
                        if (oTokens.hasMoreElements()) {
                            aToken = oTokens.nextElement().toString();  // Skip uom
                        }
                        if (oTokens.hasMoreElements())
                        {
                            aToken = oTokens.nextElement().toString();
                            if (aToken.equalsIgnoreCase("LB"))
                            {
                                joObservation.put("batteryHealth", "LB");
                            }
                            else
                            {
                                joObservation.put("batteryHealth", "OK");
                            }
                            if (oTokens.hasMoreElements()) {
                                aToken = oTokens.nextElement().toString();  // Skip uom
                            }
                        }
                        else
                        {
                            joObservation.put("batteryHealth", "OK");
                        }
                    }
                    continue;
                }
                else
                {
                    // Gas alert
                    if (healthOrAlertToken.equalsIgnoreCase("lvl"))
                    {
                        joObservation.put("gasAlert", aValue);
                    }
                    continue;
                }
            }
            // Alerts - any order
            if (aToken.equalsIgnoreCase("LB"))
            {
                joObservation.put("batteryAlert", "LB");
            }
            if (aToken.equalsIgnoreCase("R"))
            {
                joObservation.put("resetAlert", "R");
            }
            if (aToken.equalsIgnoreCase("SF"))
            {
                joObservation.put("deviceAlert", "SF");
            }

            // Skip UOM
            if (oTokens.hasMoreElements())
                oTokens.nextElement();
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
