package com.itron.enablement.sdk.mqtt.payload;

import co.nstant.in.cbor.CborException;
import com.itron.enablement.sdk.mqtt.util.PayloadUtilities;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.json.JSONArray;
import org.json.JSONObject;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.StringTokenizer;
import java.util.TimeZone;
import java.util.regex.Pattern;

/**
 * Methane (CH4) Sensor Alert Payload Transformer. Methane device payload in, Observations JSON out.
 *
 * @author Bob Luben
 */
public class Ch4PayloadTransformer
{
    private static final Logger logger = LogManager.getLogger(Ch4PayloadTransformer.class.getName());


    public String buildPayload(byte[] payload, String macId)
    {
        final String macIdLabel = "macId";

        PayloadUtilities pu = new PayloadUtilities();
        String payloadAsHexStr = pu.bytesToHexString(payload);
        logger.debug("CBOR Raw CH4 Payload Hex: <".concat(payloadAsHexStr).concat(">"));

        // Strip off the CBOR topic wrapper
        String innerPayloadAsStr;
        try
        {
            innerPayloadAsStr = pu.stripOffCborWrapperString(payload);
        }
        catch (CborException e)
        {
            logger.error("Error unwrapping CBOR payload", e);
            throw new RuntimeException(e);
        }

        logger.debug("Inner Payload Str: <".concat(innerPayloadAsStr).concat(">"));

        // Process as a stream of tokens
        StringTokenizer oTokens = new StringTokenizer(innerPayloadAsStr, ",");

        //Original payload didn't have timestamp in it
        boolean payloadHasTimestamp = Pattern.matches("^\\d{2}\\/\\d{2}\\/\\d{2}\\/\\d{2}\\/\\d{2}\\/\\d{2}.*", innerPayloadAsStr);
        Date messageReceivedTs = new Date();

        Date observationTime = null;
        if (!payloadHasTimestamp)
        {
            observationTime = messageReceivedTs;
        } else {
            String ts = oTokens.nextToken();
            oTokens.nextToken(); //skip uom
            SimpleDateFormat cborDf = new SimpleDateFormat("dd/MM/yy/HH/mm/ss");
            cborDf.setTimeZone(TimeZone.getTimeZone("UTC"));
            try {
                observationTime = cborDf.parse(ts);
            } catch (ParseException e) {
                logger.error("Error parsing timestamp", e);
                throw new RuntimeException(e);
            }
        }

        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'");
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));

        // Start JSON object with timestamp
        JSONObject joObservation = new JSONObject();
        joObservation.put("timestamp", sdf.format(observationTime));
        joObservation.put("messageReceivedTimestamp", sdf.format(messageReceivedTs));

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
                    if (oTokens.hasMoreElements()) {
                        aToken = oTokens.nextElement().toString();
                        if (aToken.equalsIgnoreCase("NG"))
                        {
                            joObservation.put("deviceHealth", "NG");
                        }
                        else
                            {
                            joObservation.put("deviceHealth", "OK");
                        }
                        if (oTokens.hasMoreElements())
                        {
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
                            if (oTokens.hasMoreElements())
                            {
                                aToken = oTokens.nextElement().toString();  // Skip uom
                            }
                        }
                        else
                            {
                            joObservation.put("batteryHealth", "OK");
                        }
                    }
                    continue;
                } else
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

        // Add the device mac address
        joObservation.put(macIdLabel, macId);

        // Build payload JSON
        JSONArray joaObservations = new JSONArray();
        joaObservations.put(joObservation);

        JSONObject jo = new JSONObject();
        jo.put("observations", joaObservations);

        String json = jo.toString();
        logger.debug("Transformed CH4 Observation: {}", json);
        return json;
    }

}
