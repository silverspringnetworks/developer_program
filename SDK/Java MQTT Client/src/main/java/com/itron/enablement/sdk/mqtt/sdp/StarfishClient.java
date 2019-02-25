package com.itron.enablement.sdk.mqtt.sdp;

import com.itron.enablement.sdk.mqtt.util.MqttSdpConfiguration;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import javax.net.ssl.HttpsURLConnection;
import java.io.DataOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;

import java.net.URL;
import java.lang.reflect.Method;


public class StarfishClient
{
    private static final Logger log = LogManager.getLogger(StarfishClient.class.getName());

    private MqttSdpConfiguration bridgeConfiguration;


    public StarfishClient(MqttSdpConfiguration bridgeConfig)
    {
        this.bridgeConfiguration = bridgeConfig;
    }


    // Starfish Observations POST
    public void sendObservations(String deviceId, String payload, String token) throws Exception
    {
        String observationsUrl = getBridgeConfiguration().getSdp_observations_endpoint();
        String fullUrl = observationsUrl + "/" + deviceId + "/observations";

        log.trace("Starfish Observations URL: {} ",fullUrl);
        URL obj = new URL(fullUrl);
        HttpsURLConnection con = (HttpsURLConnection) obj.openConnection();

        // Setup request
        con.setRequestMethod("GET");
        con.setRequestProperty("Content-Type", "application/json");
        con.setRequestProperty("Accept", "application/json");
        con.setRequestProperty("Authorization", token);

        // Send request
        con.setDoOutput(true);
        DataOutputStream wr = new DataOutputStream(con.getOutputStream());
        wr.writeBytes(payload);
        wr.flush();
        wr.close();

        // Read response
        int httpStatus = con.getResponseCode();
        if (httpStatus != 201)
        {
            log.error("Starfish observations call failed: {}", httpStatus);
            String response = con.getResponseMessage();
            log.debug("Starfish observations response: {}", response);
        }
        else
        {
            log.debug("Starfish Observations Status: {} ",httpStatus);
        }
        con.disconnect();
    }


    // Starfish Observations POST
    public String getDevices(String token) throws Exception
    {
        String devicesUrl = getBridgeConfiguration().getSdp_observations_endpoint();

        log.trace("Starfish Devices URL: {} ",devicesUrl);
        URL obj = new URL(devicesUrl);
        HttpsURLConnection con = (HttpsURLConnection) obj.openConnection();

        // Setup request
        con.setRequestMethod("GET");
        con.setRequestProperty("Content-Type", "application/json");
        con.setRequestProperty("Accept", "application/json");
        con.setRequestProperty("Authorization", token);

        // Give it 2m to respond
        con.setReadTimeout(120*1000);
        con.connect();

        // Check status
        int httpStatus = con.getResponseCode();
        if (httpStatus == 404)
        {
            log.error("Starfish devices call failed with status: {}", httpStatus);
            con.disconnect();
            return "{}";
        }
        if (httpStatus != 200)
        {
            log.error("Starfish devices call failed with status: {}", httpStatus);
            con.disconnect();
            return null;
        }

        // Read response (should be JSON devices payload)
        InputStream is = con.getInputStream();
        InputStreamReader isr = new InputStreamReader(is);
        BufferedReader br = new BufferedReader(isr);

        String line = null;
        StringBuilder sb = new StringBuilder();
        while ((line = br.readLine()) != null)
        {
            sb.append(line + "\n");
        }
        String devices = sb.toString();
        br.close();
        //log.debug("Starfish Devices Response: {} ",devices);
        con.disconnect();

        return devices;
    }


    public MqttSdpConfiguration getBridgeConfiguration() {
        return bridgeConfiguration;
    }

    public void setBridgeConfiguration(MqttSdpConfiguration bridgeConfiguration) {
        this.bridgeConfiguration = bridgeConfiguration;
    }
}
