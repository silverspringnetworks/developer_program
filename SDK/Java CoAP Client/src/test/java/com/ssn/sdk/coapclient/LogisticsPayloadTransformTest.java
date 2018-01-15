package com.ssn.sdk.coapclient;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;
import org.json.JSONObject;
import org.json.JSONArray;

/**
 * Created by Jose Olcese on 1/12/2018.
 */
public class LogisticsPayloadTransformTest extends TestCase
{
    /**
     * Create the test case
     *
     * @param testName name of the test case
     */
    public LogisticsPayloadTransformTest( String testName )
    {
        super( testName );
    }

    /**
     * @return the suite of tests being tested
     */
    public static Test suite()
    {
        return new TestSuite( LogisticsPayloadTransformTest.class );
    }

    /**
     * Test the Logistics payload transformer.
     */
    public void testbuildPayloadLogistics()
    {
        Integer level;
        String sValue;
        int iValue;
        String jsonResult;

        LogisticsPayloadTransformer pt = new LogisticsPayloadTransformer();

        jsonResult = pt.buildPayload("abc123".getBytes(), "00135005FF0662E1");

        sValue = getStrObservationAttribute(jsonResult, "palletteMacAddress");
        assertEquals("00135005FF0662E1", sValue);

        sValue = getStrObservationAttribute(jsonResult, "apMacAddress");
        assertEquals("", sValue);

        sValue = getStrObservationAttribute(jsonResult, "sensorData");
        assertEquals("YWJjMTIz", sValue); // base64("abc123")= "YWJjMTIz"

    }


    private String getStrObservationAttribute(String jsonResult, String attributeName)
    {
        String value = null;

        JSONObject jo = new JSONObject(jsonResult);
        value = jo.getString(attributeName);

        return value;
    }


    private int getIntObservationAttribute(String jsonResult, String attributeName) {
        int value = 0;

        JSONObject jo = new JSONObject(jsonResult);
        JSONArray joa = jo.getJSONArray("observations");
        for (int i = 0; i < joa.length(); i++)
        {
            value = joa.getJSONObject(i).getInt(attributeName);
        }
        return value;
    }
}
