package com.itron.enablement.sdk.mqtt.payload;

import co.nstant.in.cbor.CborException;
import com.itron.enablement.sdk.mqtt.util.PayloadUtilities;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.JSONArray;

import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.TimeZone;


/**
 * Lnic and Pole Tile IoT device Payload Transformer. LNIC device payload in, Observations JSON out.
 *
 * @author Bob Luben
 */
public class LnicPayloadDecoder
{
    private static final Logger logger = LogManager.getLogger(LnicPayloadTransformer.class.getName());


    // A payload consists of one or more LNIC device files. A file consists of one or more sensor data points.
    public List<LnicObservations> processPayload(byte[] payload)
    {
        PayloadUtilities pu = new PayloadUtilities();

        // Get the json mapping
        final String mapResourceName = "tlv2jsonmap.json";
        JSONObject tlvmap = initTlv2JsonMap(mapResourceName);

        // Strip off the CBOR topic wrapper
        byte[] innerPayload;
        try
        {
            innerPayload = pu.stripOffCborWrapperByte(payload);
        }
        catch (CborException e)
        {
            logger.error("Error unwrapping CBOR payload", e);
            throw new RuntimeException(e);
        }
        String payloadAsHexStr = pu.bytesToHexString(innerPayload);
        logger.debug("Inner LNIC Payload Hex: <".concat(payloadAsHexStr).concat(">"));

        List<LnicObservations> lolist = decodeSensorData(innerPayload, tlvmap);
        return lolist;
    }


    public List<LnicObservations> decodeSensorData(byte[] buffer, JSONObject tlvmap)
    {
        List<LnicObservations> lolist = new ArrayList<LnicObservations>();
        int offset = 0;

        // Process a device file. One or more.
        while (offset < buffer.length)
        {
            // Set up to decode payload
            LnicObservations lo = new LnicObservations();

            // Read and process the header part of the payload
            offset = decodePayloadHeader(buffer, offset, lo, tlvmap);

            // Read and process the data part of the payload
            offset = decodePayloadData(buffer, offset, lo, tlvmap);

            lolist.add(lo);
        }

        return lolist;
    }


    // Process one observation set (one or more TLV's) from the LNIC payload
    public int decodePayloadData(byte[] buffer, int offset, LnicObservations lo, JSONObject tlvmap)
    {
        int payloadOffset = offset + lo.getLengthOfDataPart();

        while (offset < payloadOffset)
        {
            long type = getUInt16BE(buffer, offset);
            String typeasstr = String.format("%04d", type);
            logger.debug("TLV Tag: {}", typeasstr);

            JSONObject tlventry = tlvmap.getJSONObject(typeasstr);
            offset += 2;

            if (offset + buffer[offset] > buffer.length)
            {
                logger.error("Invalid length at buffer offset: ".concat(Long.toString(offset)));
                return -1;
            }

            String datatype = tlventry.getString("type");
            String name = tlventry.getString("name");
            double value;
            double valuearray[];
            switch (datatype)
            {
                case "number":
                    switch (name)
                    {
                        case "temperature":
                            value = getNumber(buffer, offset+1, buffer[offset], tlventry);
                            lo.setTemperature(value);
                            lo.setTemperatureLabel(new String(name));
                            break;
                        case "humidity":
                            value = getNumber(buffer, offset+1, buffer[offset], tlventry);
                            lo.setHumidity(value);
                            lo.setHumidityLabel(new String(name));
                            break;
                        case "shock_sum":
                            value = getNumber(buffer, offset+1, buffer[offset], tlventry);
                            lo.setShocksum(value);
                            lo.setShocksumLabel(new String(name));
                            break;
                    }
                    break;
                case "numberarray":
                    switch (name)
                    {
                        case "shock":
                            valuearray = getArrayOfNumber(buffer, offset+1, buffer[offset], tlventry);
                            lo.setShock(valuearray);
                            lo.setShockLabel(new String(name));
                            lo.setShockElementLabels(getElementArrayNames(tlventry));
                            break;
                        case "tilt":
                            valuearray = getArrayOfNumber(buffer, offset+1, buffer[offset], tlventry);
                            lo.setTilt(valuearray);
                            lo.setTiltLabel(name);
                            lo.setTiltElementLabels(getElementArrayNames(tlventry));
                            break;
                    }
                    break;
                case "lookup":
                    // Alert Value
                    int alertvalue = getAlert(buffer, offset+1);
                    lo.setAlertValue(alertvalue);
                    lo.setAlertLabel(new String(name));
                    // Alert Name
                    String alertvalueasstr = String.format("%08X", alertvalue);
                    JSONObject alertnames = tlventry.getJSONObject("lookup");
                    String alertname = alertnames.getString(alertvalueasstr);
                    lo.setAlertName(alertname);
                    break;
                default:
                    logger.error("Unexpected Type at buffer offset: ".concat(Long.toString(offset)));
                    return -1;
            }
            offset += (buffer[offset] + 1);
        }
        return offset;
    }


    private String[] getElementArrayNames(JSONObject tlventry)
    {
        JSONArray elementnames = tlventry.getJSONArray("elementnames");
        int numberofelements = tlventry.getInt("numberofelements");
        String elementlabels[] = new String[numberofelements];
        for (int indx=0 ; indx < numberofelements ; indx++ )
        {
            elementlabels[indx] = elementnames.getString(indx);
        }
        return elementlabels;
    }


    public int decodePayloadHeader(byte[] buffer, int offset, LnicObservations lo, JSONObject tlvmap)
    {
        // Get the head part of the TLV map
        JSONArray header = tlvmap.getJSONArray("head");
        int head_element_count = header.length();

        // Process all header elements. Allows us to keep offset correct.
        for( int indx=0; indx < head_element_count; indx++ )
        {
            JSONObject header_element = header.getJSONObject(indx);
            String header_type = header_element.getString("type");
            long field_value = 0;
            switch (header_type)
            {
                case "timemillis":
                    field_value = getUInt32BE(buffer, offset);
                    offset += 4;
                    break;
                case "uint32":
                    field_value = getUInt32BE(buffer, offset);
                    offset += 4;
                    break;
                case "uint16":
                    field_value = getUInt16BE(buffer, offset);
                    offset += 2;
                    break;
                case "uint8":
                    field_value = getUInt8BE(buffer, offset);
                    offset += 1;
                    break;
            }
            // Extract the parts we are interested in
            String header_field = header_element.getString("h_field");
            switch (header_field)
            {
                case "timestamp":
                    String timestamp = formatTimestamp(field_value);
                    lo.setObservationTimeStamp(timestamp);
                    lo.setTimestampLabel(new String("timestamp"));
                    break;
                case "length":
                    int dataLength = (int)(field_value-1);
                    lo.setLengthOfDataPart(dataLength);
                    break;
            }
        }

        return offset;
    }


    // Helper Methods
    //

    /**
     * Get a number
     *
     * @param buffer
     * @param offset
     * @param length
     * @param tlventry
     * @return
     */
    public double getNumber(byte[] buffer, int offset, int length, JSONObject tlventry)
    {
        double result = 0.0;
        double multiplier = tlventry.getDouble("multiplier");

        // Sensor data is in 2's compliment. Java assumes numbers are in 2's compliment.
        // Just need to make sure that the Java data type has the same number of bits as the sensor data.
        switch (length)
        {
            case 4:
                int value32 = getUInt32BE(buffer, offset);
                result = ((double)value32)*multiplier;
                break;
            case 2:
                short value16 = getUInt16BE(buffer, offset);
                result = ((double)value16)*multiplier;
                break;
            case 1:
                byte value8 = getUInt8BE(buffer, offset);
                result = ((double)value8)*multiplier;
                break;
        }
        return result;
    }


    public double[] getArrayOfNumber(byte[] buffer, int offset, int length, JSONObject tlventry)
    {
        int value32;
        double multiplier = tlventry.getDouble("multiplier");

        int numberofelements = tlventry.getInt("numberofelements");
        double[] result = new double[numberofelements];

        // Decode each element. Note that elements always take 4 bytes. Compute it anyway.
        int elementlength = length/numberofelements;
        for (int indx=0 ; indx < numberofelements ; indx++)
        {
            value32 = getUInt32BE(buffer, offset+indx*elementlength);
            result[indx] = ((double)value32)*multiplier;
        }
        return result;
    }


    public int getAlert(byte[] buffer, int offset)
    {
        // Simple - just get the 32 bit value
        return getUInt32BE(buffer, offset);
    }


    /**
     * Format a Unix timestamp.
     */
    public String formatTimestamp(long unixTimestamp)
    {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'");
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
        Date dt = new Date(unixTimestamp*1000);
        String timestamp = sdf.format(dt);
        logger.debug("Observation Timestamp: {}", timestamp);

        return timestamp;
    }


    public int getUInt32BE(byte[] buffer, int offset)
    {
        int indx = offset;
        int value;

        value = 0x000000ff & buffer[indx++];
        value = (value << 8) | (0x000000ff & buffer[indx++]);
        value = (value << 8) | (0x000000ff & buffer[indx++]);
        value = (value << 8) | (0x000000ff & buffer[indx]);
        return value;
    }


    public short getUInt16BE(byte[] buffer, int offset)
    {
        int indx = offset;
        short value;

        value = (short)(0x00ff & buffer[indx++]);
        value = (short)((value << 8) | (0x00ff & buffer[indx]));
        return value;
    }


    public byte getUInt8BE(byte[] buffer, int offset)
    {
        byte value;

        value = (byte)(0xff & buffer[offset]);
        return value;
    }


    public JSONObject initTlv2JsonMap(String mapResourceName)
    {
        // Get the TLV2Json mapping file. It's a resource file on the classpath.
        PayloadUtilities pu = new PayloadUtilities();
        String tlv2jsonmap;
        try
        {
            ClassLoader classLoader = getClass().getClassLoader();
            InputStream is = classLoader.getResourceAsStream(mapResourceName);
            tlv2jsonmap = pu.convertStreamToString(is);
        }
        catch (Exception excpn)
        {
            logger.error("Failed to read tlv2jsonmap file: {}", excpn.getMessage());
            logger.error("Exception: ", excpn);
            return null;
        }

        // Parse the TLV mapping
        JSONObject jo;
        try
        {
            jo = new JSONObject(tlv2jsonmap);
        }
        catch (JSONException excpn)
        {
            logger.error("Failed to parse tlv to json mapping file: {}", excpn.getMessage());
            return null;
        }
        return jo;
    }
}
