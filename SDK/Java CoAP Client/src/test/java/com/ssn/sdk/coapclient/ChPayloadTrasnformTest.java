package com.ssn.sdk.coapclient;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;
import org.json.JSONObject;
import org.json.JSONArray;

/**
 * Created by bluben on 7/14/17.
 */
public class ChPayloadTrasnformTest extends TestCase
{
    /**
     * Create the test case
     *
     * @param testName name of the test case
     */
    public ChPayloadTrasnformTest( String testName )
    {
        super( testName );
    }

    /**
     * @return the suite of tests being tested
     */
    public static Test suite()
    {
        return new TestSuite( ChPayloadTrasnformTest.class );
    }

    /**
     * Test the Alert payload transformer.
     */
    public void testbuildPayloadChAlert()
    {
        Integer level;
        String sValue;
        int iValue;
        String jsonResult;

        ChAlertPayloadTransformer pt = new ChAlertPayloadTransformer();

        jsonResult = pt.buildPayload("10,lvl");
        iValue = getIntObservationAttribute(jsonResult, "gasAlert");
        level = new Integer(iValue);
        assertEquals("10", level.toString());

        jsonResult = pt.buildPayload("LB,a");
        sValue = getStrObservationAttribute(jsonResult, "batteryAlert");
        assertEquals("LB", sValue);

        jsonResult = pt.buildPayload("R,a");
        sValue = getStrObservationAttribute(jsonResult, "resetAlert");
        assertEquals("R", sValue);

        jsonResult = pt.buildPayload("SF,a");
        sValue = getStrObservationAttribute(jsonResult, "deviceAlert");
        assertEquals("SF", sValue);


        jsonResult = pt.buildPayload("20,lvl,LB,a,SF,a,R,a");
        iValue = getIntObservationAttribute(jsonResult, "gasAlert");
        level = new Integer(iValue);
        assertEquals("20", level.toString());
        sValue = getStrObservationAttribute(jsonResult, "batteryAlert");
        assertEquals("LB", sValue);
        sValue = getStrObservationAttribute(jsonResult, "resetAlert");
        assertEquals("R", sValue);
        sValue = getStrObservationAttribute(jsonResult, "deviceAlert");
        assertEquals("SF", sValue);

        jsonResult = pt.buildPayload("20,lvl,R,a");
        iValue = getIntObservationAttribute(jsonResult, "gasAlert");
        level = new Integer(iValue);
        assertEquals("20", level.toString());
        sValue = getStrObservationAttribute(jsonResult, "resetAlert");
        assertEquals("R", sValue);


        jsonResult = pt.buildPayload("20,lvl,LB,a,R,a");
        iValue = getIntObservationAttribute(jsonResult, "gasAlert");
        level = new Integer(iValue);
        assertEquals("20", level.toString());
        sValue = getStrObservationAttribute(jsonResult, "batteryAlert");
        assertEquals("LB", sValue);
        sValue = getStrObservationAttribute(jsonResult, "resetAlert");
        assertEquals("R", sValue);
    }


    public void testbuildPayloadChHealth1()
    {
        Integer level;
        String sValue;
        int iValue;
        String jsonResult;

        ChAlertPayloadTransformer pt = new ChAlertPayloadTransformer();

        jsonResult = pt.buildPayload("101,h,OK,s,OK,s");
        iValue = getIntObservationAttribute(jsonResult, "hoursSincePowerUp");
        level = new Integer(iValue);
        assertEquals("101", level.toString());
        sValue = getStrObservationAttribute(jsonResult, "deviceHealth");
        assertEquals("OK", sValue);
        sValue = getStrObservationAttribute(jsonResult, "batteryHealth");
        assertEquals("OK", sValue);
    }


    public void testbuildPayloadChHealth2()
    {
        Integer level;
        String sValue;
        int iValue;
        String jsonResult;

        ChAlertPayloadTransformer pt = new ChAlertPayloadTransformer();

        jsonResult = pt.buildPayload("101,h,NG,s");
        iValue = getIntObservationAttribute(jsonResult, "hoursSincePowerUp");
        level = new Integer(iValue);
        assertEquals("101", level.toString());
        sValue = getStrObservationAttribute(jsonResult, "deviceHealth");
        assertEquals("NG", sValue);
        sValue = getStrObservationAttribute(jsonResult, "batteryHealth");
        assertEquals("OK", sValue);
    }


    public void testbuildPayloadChHealth3()
    {
        Integer level;
        String sValue;
        int iValue;
        String jsonResult;

        ChAlertPayloadTransformer pt = new ChAlertPayloadTransformer();

        jsonResult = pt.buildPayload("0,h,NG,s,LB,s");
        iValue = getIntObservationAttribute(jsonResult, "hoursSincePowerUp");
        level = new Integer(iValue);
        assertEquals("0", level.toString());
        sValue = getStrObservationAttribute(jsonResult, "deviceHealth");
        assertEquals("NG", sValue);
        sValue = getStrObservationAttribute(jsonResult, "batteryHealth");
        assertEquals("LB", sValue);
    }


    public void testbuildPayloadChHealthUsingHealthClass()
    {
        Integer level;
        String sValue;
        int iValue;
        String jsonResult;

        ChHealthPayloadTransformer pt = new ChHealthPayloadTransformer();

        jsonResult = pt.buildPayload("101,h,OK,s,OK,s");
        iValue = getIntObservationAttribute(jsonResult, "hoursSincePowerUp");
        level = new Integer(iValue);
        assertEquals("101", level.toString());
        sValue = getStrObservationAttribute(jsonResult, "deviceHealth");
        assertEquals("OK", sValue);
        sValue = getStrObservationAttribute(jsonResult, "batteryHealth");
        assertEquals("OK", sValue);
    }


    private String getStrObservationAttribute(String jsonResult, String attributeName)
    {
        String value = null;

        JSONObject jo = new JSONObject(jsonResult);
        JSONArray joa = jo.getJSONArray("observations");
        for (int i=0 ; i < joa.length(); i++)
        {
            value = joa.getJSONObject(i).getString(attributeName);
            if (value == null || value.length() == 0)
            {
                continue;
            }
        }
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
