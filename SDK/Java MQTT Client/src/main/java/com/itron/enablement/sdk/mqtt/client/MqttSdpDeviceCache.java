package com.itron.enablement.sdk.mqtt.client;

import com.itron.enablement.sdk.mqtt.sdp.StarfishClient;
import com.itron.enablement.sdk.mqtt.sdp.TokenClient;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.ConcurrentHashMap;

import com.itron.enablement.sdk.mqtt.util.MqttSdpConfiguration;
import com.itron.enablement.sdk.mqtt.util.PayloadUtilities;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONTokener;


public class MqttSdpDeviceCache
{
    private static final Logger logger = LogManager.getLogger(MqttSdpDeviceCache.class.getName());

    private static final ConcurrentMap<String,String> deviceCache = new ConcurrentHashMap<String, String>();


    public static boolean initializeDeviceCache(MqttSdpConfiguration bridgeConfiguration)
    {
        logger.debug("Building Device Cache");

        // Fet token
        String token;
        PayloadUtilities pu = new PayloadUtilities();
        token = pu.getStarfishToken(bridgeConfiguration);
        if (token == null)
        {
            logger.info("Failed to get token - skipping read of devices");
            return false;
        }

        // Get devices
        StarfishClient sc = new StarfishClient(bridgeConfiguration);
        String devices;
        try
        {
            devices = sc.getDevices(token);
        }
        catch(Exception excptn)
        {
            logger.error("Exception getting devices: {}", excptn);
            return false;
        }

        // Error getting devices
        if (devices == null)
        {
            return false;
        }

        // No devices returned
        if (devices.contentEquals("{}"))
        {
            return true;
        }


        // Process devices JSON. Pick out the nic_macID to id associations.
        JSONArray array;
        try
        {
            JSONTokener tokener = new JSONTokener(devices);
            JSONObject root = new JSONObject(tokener);
            array = root.getJSONArray("devices");
        }
        catch (Exception excptn)
        {
            logger.error("Exception in device JSON: {}", excptn);
            return false;
        }

        int deviceCount = array.length();
        logger.debug("Devices Found ins device repo: {}", deviceCount);
        for (int indx=0; indx < deviceCount; indx++)
        {
            // Skip past bogus device entries. Usually this means no nic_macID. This is normal.
            try
            {
                JSONObject device = array.getJSONObject(indx);
                JSONObject domainInfo = device.getJSONObject("domainInfo");

                String id = device.getString("id");
                logger.trace("id: {}", id);

                String nic_macID = domainInfo.getString("nic_macID");
                logger.trace("nic_macID: {}", nic_macID);

                getDeviceCache().putIfAbsent(nic_macID, id);
            }
            catch (Exception excptn)
            {
                logger.warn("Exception parsing device - skipping. This is normal: {}", excptn.getMessage());
            }
        }
        return true;
    }

    public static ConcurrentMap<String, String> getDeviceCache() {
        return deviceCache;
    }
}
