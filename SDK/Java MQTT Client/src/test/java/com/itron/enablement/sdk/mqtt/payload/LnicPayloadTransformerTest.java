package com.itron.enablement.sdk.mqtt.payload;

import org.testng.Assert;
import org.testng.annotations.Test;

import java.util.List;
import javax.xml.bind.DatatypeConverter;


/**
 * Created by bluben on 3/5/18.
 */
public class LnicPayloadTransformerTest
{
    private final String payload1 = "a200646c6e696301582557d8981400011a76001910000200010209670003014c00050c00000130000002d00000023c";
    private final String payload2 = "a200646c6e696301582a57d8981400011a76001e100002000604000010010004040000000200020c00000130000002d00000023c";
    private final String payload3 = "a200646c6e696301584a57d8981400011a76001910000200010209670003014c00050c00000130000002d00000023c57d8981400011a76001e100002000604000010010004040000000200020c00000130000002d00000023c";
    private final String payload4 = "a200646c6e696301581c5ABAF72000000301001000000200050C000000220000043500000031";
    // Note: old deviceId: 0a371895-e5e8-4bc9-b4e9-504385e45ab5
    private final String expectedJson1 = "{\"observations\":[{\"temperature\":24.07,\"humidity\":76,\"tilt\":{\"x\":304,\"y\":720,\"z\":572},\"timestamp\":\"2016-09-14T00:21:40.000Z\"}],\"macId\":\"001350050047dd3b\",\"deviceId\":\"001350050047dd3b\"}";
    private final String expectedJson2 = "{\"observations\":[{\"shock_sum\":4097,\"alert\":\"shock\",\"shock\":{\"x\":304,\"y\":720,\"z\":572},\"timestamp\":\"2016-09-14T00:21:40.000Z\"}],\"macId\":\"001350050047dd3b\",\"deviceId\":\"001350050047dd3b\"}";

    private final String deviceId = "0a371895-e5e8-4bc9-b4e9-504385e45ab5";
    private final String macId = "001350050047dd3b";


    @Test()
    public void testPayloadTransformer1()
    {
        payloadTestCommon(payload1, expectedJson1, deviceId, macId);
    }

    private void payloadTestCommon(String payload, String expectedJson, String deviceId, String macId) {
        LnicPayloadTransformer pt = new LnicPayloadTransformer();
        byte[] buffer = toByteArray(payload);

        List<String> loJsonList = pt.buildPayload(buffer, macId);
        Assert.assertEquals(loJsonList.size(), 1);
        String lojson = loJsonList.get(0);
        Assert.assertEquals(lojson, expectedJson);
    }


    @Test()
    public void testPayloadTransformer2()
    {
        payloadTestCommon(payload2, expectedJson2, deviceId, macId);
    }


    @Test()
    public void testPayloadTimestampFail()
    {
        LnicPayloadTransformer pt = new LnicPayloadTransformer();
        byte[] buffer = toByteArray(payload4);

        List<String> loJsonList = pt.buildPayload(buffer, macId);
        Assert.assertEquals(loJsonList.size(), 1);
        String lojson = loJsonList.get(0);
        //Assert.assertEquals(lojson, expectedJson2);
    }


    @Test()
    public void testPayloadTransformer3()
    {
        LnicPayloadTransformer pt = new LnicPayloadTransformer();
        byte[] buffer = toByteArray(payload3);

        List<String> loJsonList = pt.buildPayload(buffer, macId);
        Assert.assertEquals(loJsonList.size(), 2);
        String lojson1 = loJsonList.get(0);
        Assert.assertEquals(lojson1, expectedJson1);
        String lojson2 = loJsonList.get(1);
        Assert.assertEquals(lojson2, expectedJson2);
    }


    // Helper Methods
    //
    private static byte[] toByteArray(String s)
    {
        return DatatypeConverter.parseHexBinary(s);
    }
}
