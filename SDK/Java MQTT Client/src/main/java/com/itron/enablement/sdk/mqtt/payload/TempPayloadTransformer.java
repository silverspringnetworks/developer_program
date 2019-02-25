package com.itron.enablement.sdk.mqtt.payload;

import com.itron.enablement.sdk.mqtt.util.PayloadUtilities;
import co.nstant.in.cbor.CborDecoder;
import co.nstant.in.cbor.CborException;
import co.nstant.in.cbor.model.DataItem;
import co.nstant.in.cbor.model.UnsignedInteger;
import org.apache.logging.log4j.LogManager;
import org.json.JSONArray;
import org.json.JSONObject;

import java.io.ByteArrayInputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.TimeZone;


/**
 * Temperature Sensor Payload Transformer. Device payload in, Observations JSON out.
 * Note that we support both CBOR wrapped device payloads and core payload: <epoch>,<temp>,<unit>
 *
 * @author Bob Luben
 */
public class TempPayloadTransformer
{
    private static final org.apache.logging.log4j.Logger logger = LogManager.getLogger(TempPayloadTransformer.class.getName());


    public String buildPayload(byte[] payloadAsByteArray)
    {
        final String obsRootLabel   = "observations";
        final String timestampLabel = "timestamp";
        final String tempLabel      = "temperature";
        String payloadAsStr;

        // This code provides an example of how to strip off a CBOR wrapper.
        // If the payload is CBOR decode it. Note that we expect the a CBOR wrapped core payload.
        // If the payload is not CBOR wrapped than we assume it's just a core payload.
        // Depends on how the CoAP Server is configured.

        // Simple way to detect if the payload is CBOR
        int aByte0 = (int)payloadAsByteArray[0] & 0xFF;
        int aByte1 = (int)payloadAsByteArray[1] & 0xFF;
        if (aByte0 == 0xA2 && aByte1 == 0X00)
        {
            PayloadUtilities pu = new PayloadUtilities();
            String payloadAsHexStr = pu.bytesToHexString(payloadAsByteArray);
            logger.debug("CBOR Raw Temp Payload Hex: <".concat(payloadAsHexStr).concat(">"));

            ByteArrayInputStream bais = new ByteArrayInputStream(payloadAsByteArray);
            List<DataItem> dataItems;
            try
            {
                dataItems = new CborDecoder(bais).decode();
            }
            catch (CborException e)
            {
                logger.error("Error unwrapping CBOR Temp payload", e);
                throw new RuntimeException(e);
            }
            co.nstant.in.cbor.model.Map payloadMap = (co.nstant.in.cbor.model.Map) dataItems.get(0);
            DataItem wrappedPayload = payloadMap.get(new UnsignedInteger(1));
            payloadAsStr = wrappedPayload.toString();
            logger.debug("Actual Temp Payload: <".concat(payloadAsStr).concat(">"));
        }
        else
        {
            payloadAsStr = new String(payloadAsByteArray);
        }

        // Transform the core payload. This generates a Starfish Data Platform "observations" JSON payload.
        // Core payload: <epoch>,<temp>,<unit>
        String[] parts = payloadAsStr.split(",", 3);

        Long ts = new Long(parts[0]);
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'");
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
        Date dt = new Date(ts);
        String timestamp = sdf.format(dt);
        logger.debug("timestamp: {}", timestamp);

        // Convert the observation
        JSONObject joObservation = new JSONObject();
        joObservation.put(timestampLabel, timestamp);
        joObservation.put(tempLabel, parts[1]);

        JSONArray joaObservations = new JSONArray();
        joaObservations.put(joObservation);

        // Build payload
        JSONObject jo = new JSONObject();
        jo.put(obsRootLabel, joaObservations);

        String json = jo.toString();
        logger.debug("Starfish payload: {}", json);

        return json;
    }
}
