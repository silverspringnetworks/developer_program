package com.itron.enablement.sdk.mqtt.payload;

import javax.xml.bind.DatatypeConverter;

import org.json.JSONObject;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.util.List;

/**
 * Created by bluben on 2/28/18.
 */
public class LnicPayloadDecoderTest
{
    private static final String payload1 = "57d8981400011a76001910000200010209670003014c00050c00000130000002d00000023c";
    private static final String datapart1 = "00010209670003014c00050c00000130000002d00000023c";
    private static final long   datalength1 = 0x0019;
    private static final String datapart2 = "000604000010010004040000000200020c00000130000002d00000023c";
    private static final long   datalength2 = 0x001e;
    private static final String payload3 = "57d8981400011a76001910000200010209670003014c00050c00000130000002d00000023c57d8982800011a77001900040200010209670003014c00050c00000130000002d00000023c";
    private static final String cbor3 = "a200646c6e696301584a57d8981400011a76001910000200010209670003014c00050c00000130000002d00000023c57d8982800011a77001900040200010209670003014c00050c00000130000002d00000023c";
    private static final String payload4 = "5aa1a45000000118001000000200050c0000001c00000432000000385aa1a5110000011900170000020004040000000800050cffffffebffffffff0000048f";
    private static final String cbor4 = "a200646c6e696301583f5aa1a45000000118001000000200050c0000001c00000432000000385aa1a5110000011900170000020004040000000800050cffffffebffffffff0000048f";
    private static final String payloadts = "57d89814";
    private static final String payload16 = "0019";
    private static final String payload8 = "02";
    private static final long   unixts = 0x57d89814;
    private static final long   value16 = 0x0019;
    private static final long   value8 = 0x02;
    private static final String payloadt0001pos = "0001020967";
    private static final String payloadt0001neg = "000102e700";
    private static final String payloadt0003 = "0003014c";
    private static final String payloadt0005 = "00050c00000130000002d00000023c";
    private static final String payloadt0004 = "00040400000002";

    private static final String observationts = "2016-09-14T00:21:40.000Z";

    private final String mapResourceName = "tlv2jsonmap.json";


    @Test()
    public void testDecodePayloadHeader()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        JSONObject tlvmap = pt.initTlv2JsonMap(mapResourceName);
        LnicObservations lo = new LnicObservations();
        byte[] buffer = toByteArray(payload1);
        int offset = 0;

        // Read and process the header part of the payload
        offset = pt.decodePayloadHeader(buffer, offset, lo, tlvmap);

        // Check offset
        Assert.assertEquals(offset,13 );
        // Observation timestamp
        String resultts = lo.getObservationTimeStamp();
        Assert.assertEquals(resultts, observationts);
        // Data Length
        int resultdatalength = lo.getLengthOfDataPart();
        Assert.assertEquals(resultdatalength, datalength1-1);
    }


    @Test()
    // Test types 0x0001, 0x0003, 0x0005
    public void testDecodePayloadData1()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        JSONObject tlvmap = pt.initTlv2JsonMap(mapResourceName);
        LnicObservations lo = new LnicObservations();
        byte[] buffer = toByteArray(datapart1);
        int offset = 0;
        // Need a fake out as we don't decode the header in this test.
        lo.setLengthOfDataPart((int)datalength1-1);

        // Read and process the header part of the payload
        offset = pt.decodePayloadData(buffer, offset, lo, tlvmap);

        // Check offset
        Assert.assertEquals(offset, datalength1-1 );
        // Check decoded payload
        verifyPayload4Result(lo);
    }


    @Test()
    // Test types 0x0006, 0x0004, 0x0002
    public void testDecodePayloadData2()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        JSONObject tlvmap = pt.initTlv2JsonMap(mapResourceName);
        LnicObservations lo = new LnicObservations();
        byte[] buffer = toByteArray(datapart2);
        int offset = 0;
        // Need a fake out as we don't decode the header in this test.
        lo.setLengthOfDataPart((int)datalength2-1);

        // Read and process the header part of the payload
        offset = pt.decodePayloadData(buffer, offset, lo, tlvmap);

        // Check offset
        Assert.assertEquals(offset, datalength2-1 );

        // Check decoded payload
        double value;
        double[] valuearray;

        // Check Values
        value = lo.getShocksum();
        Assert.assertEquals(value, 4097.0);
        int alertvalue = lo.getAlertValue();
        Assert.assertEquals(alertvalue, 0x00000002);
        String alertname = lo.getAlertName();
        Assert.assertEquals(alertname, "shock");
        valuearray = lo.getShock();
        Assert.assertEquals(valuearray[0], 304.0);
        Assert.assertEquals(valuearray[1], 720.0);
        Assert.assertEquals(valuearray[2], 572.0);

        // Check Labels
        Assert.assertEquals(lo.getShocksumLabel(), "shock_sum");
        String[] elementNameArray = lo.getShockElementLabels();
        Assert.assertEquals(elementNameArray[0], "x");
        Assert.assertEquals(elementNameArray[1], "y");
        Assert.assertEquals(elementNameArray[2], "z");
    }


    @Test()
    // Payload with two observations (files) - using basic sensor payload each the same
    public void testDecodeSensorData1()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        JSONObject tlvmap = pt.initTlv2JsonMap(mapResourceName);
        byte[] buffer = toByteArray(payload3);

        // Read and process the header part of the payload
        List<LnicObservations> lolist = pt.decodeSensorData(buffer, tlvmap);

        // Expect 2 LO's
        Assert.assertEquals(lolist.size(), 2 );

        // Check each LO
        for (LnicObservations lo : lolist)
        {
            // Check decoded payload
            verifyPayload4Result(lo);
        }
    }

    @Test()
    // Payload with two observations (files) - using basic sensor payload each the same
    public void testDecodeSensorData2()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        JSONObject tlvmap = pt.initTlv2JsonMap(mapResourceName);
        byte[] buffer = toByteArray(payload4);

        // Read and process the header part of the payload
        List<LnicObservations> lolist = pt.decodeSensorData(buffer, tlvmap);
        verifyPayload4Result(lolist);
    }


    @Test()
    // Payload with two observations (files) - using basic sensor payload each the same
    public void testProcessPayloadCbor3()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        byte[] buffer = toByteArray(cbor3);

        // Read and process the header part of the payload
        List<LnicObservations> lolist = pt.processPayload(buffer);

        // Expect 2 LO's. LO's are verified in testDecodeSensorData
        Assert.assertEquals(lolist.size(), 2 );

        // Check each LO
        for (LnicObservations lo : lolist)
        {
            // Check decoded payload
            verifyPayload4Result(lo);
        }
    }


    @Test()
    // Payload with two observations (files) - using basic sensor payload each the same
    public void testProcessPayloadCbor4()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        byte[] buffer = toByteArray(cbor4);

        // Read and process the header part of the payload
        List<LnicObservations> lolist = pt.processPayload(buffer);
        verifyPayload4Result(lolist);
    }


    @Test()
    public void testGetNumber()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        // Get the json mapping
        JSONObject tlvmap = pt.initTlv2JsonMap(mapResourceName);
        JSONObject tlventry = tlvmap.getJSONObject("0001");

        // Two byte value positive
        byte[] buffer = toByteArray(payloadt0001pos);
        double result = pt.getNumber(buffer, 3, buffer[2], tlventry);
        Assert.assertEquals(result, 24.07);

        // Two byte value negative
        buffer = toByteArray(payloadt0001neg);
        result = pt.getNumber(buffer, 3, buffer[2], tlventry);
        Assert.assertEquals(result, -64.00);

        // One byte value positive
        tlventry = tlvmap.getJSONObject("0003");
        buffer = toByteArray(payloadt0003);
        result = pt.getNumber(buffer, 3, buffer[2], tlventry);
        Assert.assertEquals(result, 76.0);
    }


    @Test()
    public void testGetArrayOfNumber()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        // Get the json mapping
        JSONObject tlvmap = pt.initTlv2JsonMap(mapResourceName);
        JSONObject tlventry = tlvmap.getJSONObject("0005");

        // Tilt - three values
        byte[] buffer = toByteArray(payloadt0005);
        double[] result = pt.getArrayOfNumber(buffer, 3, buffer[2], tlventry);
        Assert.assertEquals(result[0], 304.0);
        Assert.assertEquals(result[1], 720.0);
        Assert.assertEquals(result[2], 572.0);
    }


    @Test()
    public void testGetAlert()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();

        byte[] buffer = toByteArray(payloadt0004);
        int result = pt.getAlert(buffer, 3);

        Assert.assertEquals(result, 0x00000002);
    }


    @Test()
    public void testGetUInt32BE()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        byte[] payload = toByteArray(payloadts);

        long result = pt.getUInt32BE(payload, 0);
        Assert.assertEquals(result, unixts);
    }

    @Test()
    public void testGetUInt16BE()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        byte[] payload = toByteArray(payload16);

        long result = pt.getUInt16BE(payload, 0);
        Assert.assertEquals(result, value16);
    }

    @Test()
    public void testGetUInt8BE()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        byte[] payload = toByteArray(payload8);

        long result = pt.getUInt8BE(payload, 0);
        Assert.assertEquals(result, value8);
    }


    @Test()
    public void testGetTimestamp()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        String timestamp = pt.formatTimestamp(unixts);

        Assert.assertEquals(timestamp, observationts);
    }


    /**
     * Test GetTlv2JsonMap method
     */
    @Test()
    public void testGetTlv2JsonMap()
    {
        LnicPayloadDecoder pt = new LnicPayloadDecoder();
        String mapResourceName = "tlv2jsonmap.json";

        JSONObject jo = pt.initTlv2JsonMap(mapResourceName);

        // Expect an object
        Assert.assertNotNull(jo);

        // Expect 7 name value mappings
        int namevaluemappingcount = jo.length();
        Assert.assertEquals(namevaluemappingcount, 7);
    }


    // Helper Methods
    //
    private static byte[] toByteArray(String s)
    {
        return DatatypeConverter.parseHexBinary(s);
    }


    private void verifyPayload4Result(LnicObservations lo)
    {
        double value;
        double[] valuearray;

        // Check Values
        value = lo.getTemperature();
        Assert.assertEquals(value, 24.07);
        value = lo.getHumidity();
        Assert.assertEquals(value, 76.0);
        valuearray = lo.getTilt();
        Assert.assertEquals(valuearray[0], 304.0);
        Assert.assertEquals(valuearray[1], 720.0);
        Assert.assertEquals(valuearray[2], 572.0);

        // Check Labels
        Assert.assertEquals(lo.getTemperatureLabel(), "temperature");
        Assert.assertEquals(lo.getHumidityLabel(), "humidity");
        String[] elementNameArray = lo.getTiltElementLabels();
        Assert.assertEquals(elementNameArray[0], "x");
        Assert.assertEquals(elementNameArray[1], "y");
        Assert.assertEquals(elementNameArray[2], "z");
    }

    private void verifyPayload4Result(List<LnicObservations> lolist)
    {
        // Expect 2 LO's
        Assert.assertEquals(lolist.size(), 2 );
        double[] valuearray;

        // check LO 1
        LnicObservations lo = lolist.get(0);
        valuearray = lo.getTilt();
        Assert.assertEquals(valuearray[0], 28.0);
        Assert.assertEquals(valuearray[1], 1074.0);
        Assert.assertEquals(valuearray[2], 56.0);

        // check LO 2
        lo = lolist.get(1);
        valuearray = lo.getTilt();
        Assert.assertEquals(valuearray[0], -21.0);
        Assert.assertEquals(valuearray[1], -1.0);
        Assert.assertEquals(valuearray[2], 1167.0);
        Assert.assertEquals(lo.getAlertName(), "tiltcleared");
    }
}
