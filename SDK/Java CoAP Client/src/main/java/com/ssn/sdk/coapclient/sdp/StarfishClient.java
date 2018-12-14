/*
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient.sdp;

import javax.net.ssl.HttpsURLConnection;
import java.io.DataOutputStream;
import java.net.URL;
import java.lang.reflect.Method;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 *
 * @author Bob Luben
 */
public class StarfishClient
{
    private Logger log = LoggerFactory.getLogger(this.getClass().getName());

    // Starfish Staging and Production environment observations endpoint
    private static final String stagingObservationsEndpoint = "https://api.data-platform.developer.ssni.com/api/solutions/sandbox/devices";

    // Starfish Test environment observations endpoint
    private static final String testObservationsEndpoint = "https://poc.api.dev.ssniot.cloud/api/solutions/sandbox/devices";

    // Logistics Staging and Production environment observations endpoint
    // TODO: validate this is the proper endpoint
    private static final String stagingLogisticsObservationsEndpoint = "https://logistics.dev.ssniot.cloud";

    // Starfish Test environment observations endpoint
    private static final String testLogisticsObservationsEndpoint = "https://logistics.dev.ssniot.cloud";

    // Default observations URL
    private static String observationsUrl = stagingObservationsEndpoint;

    // Class specific
    private static String clientId = null;
    private static String clientSecret = null;
    private static String deviceId = null;
    private static String apMacAddress = null;
    private static String palletMacAddress = null;

    private static boolean useTestEnvironment = false;


    /**
     *  Initializes an instance of {@link StarfishClient}.
     *
     */
    public StarfishClient(String sfClientId, String sfClientSecret, String sfDeviceId, boolean sfUseTestEnvironment )
    {
        clientId = sfClientId;
        clientSecret = sfClientSecret;
        deviceId = sfDeviceId;
        useTestEnvironment = sfUseTestEnvironment;

        // Default
        observationsUrl = stagingObservationsEndpoint;
        if (sfUseTestEnvironment)
        {
            observationsUrl = testObservationsEndpoint;
        }
    }


    public void sendObservation(String payload, String payloadTransformerName)
    {
        Object pti = null;
        Class ptc = null;
        String payloadAsJson = null;

        // Build observations payload
        try
        {
            // Use reflection to load and call the transformer class
            ptc = Class.forName(payloadTransformerName);
            pti = ptc.newInstance();
            Method method = ptc.getMethod("buildPayload", String.class);

            payloadAsJson = (String) method.invoke(pti, payload);
        }
        catch (Exception excptn)
        {
            log.error("Payload Transformer Class Exception: {}", excptn.getCause());
            log.info("Skipping send of observation to Starfish");
            return;
        }

        // Send payload to Starfish Data Platform
        log.info("Sending observation to Starfish");
        log.debug("Starfish Payload: {}", payloadAsJson);
        sendObservation2StarfishDataPlatform(deviceId, payloadAsJson, useTestEnvironment);
    }


    public void sendObservation(byte[] payload, String payloadTransformerName)
    {
        Object pti = null;
        Class ptc = null;
        String payloadAsJson;

        // Build Starfish Data Platform observations payload
        try
        {
            // Use reflection to load and call the transformer class
            ptc = Class.forName(payloadTransformerName);
            pti = ptc.newInstance();
            Method method = ptc.getMethod("buildPayload", byte[].class);

            payloadAsJson = (String) method.invoke(pti, payload);
        }
        catch (Exception excptn)
        {
            log.error("Exception in sendObservations: {}", excptn.getCause());
            log.info("Skipping send of observation to Starfish");
            return;
        }

        // Send payload to Starfish Data Platform
        log.info("Sending observation to Starfish");
        log.debug("Starfish Payload: {}", payloadAsJson);
        sendObservation2StarfishDataPlatform(deviceId, payloadAsJson, useTestEnvironment);
    }


    // Helper to send an observation payload to Starfish Data Platform
    //
    public boolean sendObservation2StarfishDataPlatform(String deviceId, String payload, boolean useTestEnvironment)
    {
        try
        {
            if (deviceId != null)
            {
                // Get a Starfish token
                TokenClient tc = new TokenClient(useTestEnvironment);
                String token;
                try
                {
                    token = tc.getApiToken(clientId, clientSecret);
                }
                catch (Exception excpn)
                {
                    log.error("Failed to get Starfish token with Exception: {}", excpn.getMessage());
                    log.error("Can't send to SDP: Dropping message!");
                    return true;
                }
                if (token == null)
                {
                    log.error("No token: Can't send to SDP: Dropping message!");
                    return true;
                }
                sendObservations(deviceId, payload, token);
            }
        }
        catch (Exception excptn)
        {
            log.error("Exception in observations call: {}", excptn.getMessage());
            log.error("Can't send to SDP: Dropping message!");
            return true;
        }
        return false;
    }


    // Do the SDP HTTP POST observations
    //
    public void sendObservations(String deviceId, String payload, String token) throws Exception
    {
        String fullUrl;

        // https://api.data-platform.developer.ssni.com/api/solutions/<solutionId>/devices/<deviceId>/observations
        fullUrl = observationsUrl + "/" + deviceId + "/observations";

        log.debug("Starfish Observations URL: {} ",fullUrl);
        URL obj = new URL(fullUrl);
        HttpsURLConnection con = (HttpsURLConnection) obj.openConnection();

        // Setup request
        con.setRequestMethod("POST");
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
            log.debug("Starfish observations call response: {}", response);
        }
        else
        {
            log.debug("Starfish Observations call status: {} ",httpStatus);
        }
        con.disconnect();
    }
}
