package com.itron.enablement.sdk.mqtt.sdp;

import com.itron.enablement.sdk.mqtt.util.MqttSdpConfiguration;
import com.itron.enablement.sdk.mqtt.util.PayloadUtilities;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.lang.reflect.Method;

public class Ch4Handler
{
    private static final Logger log = LogManager.getLogger(Ch4Handler.class.getName());
    private MqttSdpConfiguration bridgeConfiguration;

    public Ch4Handler(MqttSdpConfiguration bridgeConfig)
    {
        this.bridgeConfiguration = bridgeConfig;
    }


    public void sendObservation(byte[] payload, String payloadTransformerName, String deviceId, String nic_macID)
    {
        // Transform observations payload
        Object pti = null;
        Class ptc = null;
        String payloadJson;
        try
        {
            // Use reflection to load and call the transformer class
            ptc = Class.forName(payloadTransformerName);
            pti = ptc.newInstance();
            Method method = ptc.getMethod("buildPayload", byte[].class, String.class);

            payloadJson = (String) method.invoke(pti, payload, nic_macID);
        }
        catch (Exception excptn)
        {
            log.error("Payload Transformer Class Exception: {}", excptn.getMessage());
            log.error("Exception: {}", excptn);
            log.info("Skipping send of observation to Starfish Data Platform");
            return;
        }


        // Send observations to Starfish Data Platform
        PayloadUtilities pu = new PayloadUtilities();
        pu.sendObservation2StarfishDataPlatform(deviceId, payloadJson, getBridgeConfiguration());
    }


    private MqttSdpConfiguration getBridgeConfiguration() {
        return bridgeConfiguration;
    }
}
