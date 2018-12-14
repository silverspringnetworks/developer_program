package com.ssn.sdk.coapclient.payload;

import co.nstant.in.cbor.CborDecoder;
import co.nstant.in.cbor.CborException;
import co.nstant.in.cbor.model.ByteString;
import co.nstant.in.cbor.model.DataItem;
import co.nstant.in.cbor.model.UnsignedInteger;
import com.ssn.sdk.coapclient.util.PayloadUtilities;
//import org.apache.logging.log4j.LogManager;
//import org.apache.logging.log4j.Logger;
import org.json.JSONArray;
import org.json.JSONObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.ByteArrayInputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.TimeZone;

public class WfciPayloadTransformer
{
    private Logger logger = LoggerFactory.getLogger(this.getClass().getName());

    public String buildPayload(byte[] payload)
    {
        final String obsRootLabel = "observations";
        final String sensorData1Label = "sensordata1";
        final String sensorData2Label = "sensordata2";
        final String timestampLabel = "timestamp";


        PayloadUtilities pu = new PayloadUtilities();
        String payloadAsHexStr = pu.bytesToHexString(payload);
        logger.debug("CBOR Raw WFCI Payload Hex: <".concat(payloadAsHexStr).concat(">"));

        // Decode the CBOR payload...

        //String innerPayloadAsHexStr = pu.bytesToHexString(innerPayloadAsBytes);
        ByteArrayInputStream bais = new ByteArrayInputStream(payload);
        List<DataItem> dataItems;
        try
        {
            dataItems = new CborDecoder(bais).decode();
        }
        catch (CborException e)
        {
            logger.error("Error unwrapping CBOR payload", e);
            throw new RuntimeException(e);
        }
        co.nstant.in.cbor.model.Map payloadMap = (co.nstant.in.cbor.model.Map) dataItems.get(0);
        co.nstant.in.cbor.model.Array data = (co.nstant.in.cbor.model.Array) payloadMap.get(new UnsignedInteger(1));
        List<DataItem> aDataItemList = data.getDataItems();

        co.nstant.in.cbor.model.UnsignedInteger data1 = (co.nstant.in.cbor.model.UnsignedInteger) aDataItemList.get(0);
        ByteString data2 = (ByteString) aDataItemList.get(1);

        String data1AsStr = data1.toString();
        logger.debug("Inner Payload Part 1: <".concat(data1AsStr).concat(">"));
        String data2AsHexStr = pu.bytesToHexString(data2.getBytes());
        logger.debug("Inner Payload Part 2: <".concat(data2AsHexStr).concat(">"));


        // Add Starfish Data Platform properties...

        // Add timestamp property. This is the timestamp of when the MQTT client processes the message.
        // If the payload contains a timestamp it can be used instead.
        Date messageReceivedTs = new Date();
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'");
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));

        JSONObject joObsProperties = new JSONObject();
        joObsProperties.put(timestampLabel, sdf.format(messageReceivedTs));


        // Add sensordata property. For now value is just the hex string.
        // TODO: Expand to expand the payload into appripriate prop/value pairs as needed.
        joObsProperties.put(sensorData1Label, data1AsStr);
        joObsProperties.put(sensorData2Label, data2AsHexStr);


        // Build payload JSON
        JSONArray joaObservations = new JSONArray();
        joaObservations.put(joObsProperties);
        JSONObject rootJo = new JSONObject();
        rootJo.put(obsRootLabel, joaObservations);

        String json = rootJo.toString();
        logger.debug("Transformed WFCI Observation: {}", json);

        return json;
    }
}
