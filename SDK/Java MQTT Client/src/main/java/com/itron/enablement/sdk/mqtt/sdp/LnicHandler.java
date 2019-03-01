package com.itron.enablement.sdk.mqtt.sdp;

import com.itron.enablement.sdk.mqtt.util.MqttSdpConfiguration;
import com.itron.enablement.sdk.mqtt.util.PayloadUtilities;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.lang.reflect.Method;
import java.util.List;



public class LnicHandler
{
    private static final Logger log = LogManager.getLogger(LnicHandler.class.getName());
    private MqttSdpConfiguration bridgeConfiguration;

    public LnicHandler(MqttSdpConfiguration bridgeConfig)
    {
        this.bridgeConfiguration = bridgeConfig;
    }


    public void sendObservation(byte[] payload, String payloadTransformerName, String macId, String deviceId)
    {
        // Transform observations payload
        Object pti = null;
        Class ptc = null;
        List<String> obsJsonList;
        try
        {
            // Use reflection to load and call the transformer class
            ptc = Class.forName(payloadTransformerName);
            pti = ptc.newInstance();
            Method method = ptc.getMethod("buildPayload", byte[].class, String.class);

            obsJsonList = (List<String>) method.invoke(pti, payload, macId);
        }
        catch (Exception excptn)
        {
            log.error("Payload Transformer Class Exception: {}", excptn.getMessage());
            log.error("Exception: {}", excptn);
            log.info("Skipping send of observation to Silverlink");
            return;
        }


        // Send observations to Silverlink Data Platform
        for ( String oneLnicObservation : obsJsonList)
        {
            PayloadUtilities pu = new PayloadUtilities();
            if (pu.sendObservation2StarfishDataPlatform(deviceId, oneLnicObservation, getBridgeConfiguration()))
            {
                return;
            }
        }
    }


    private MqttSdpConfiguration getBridgeConfiguration() {
        return bridgeConfiguration;
    }
}