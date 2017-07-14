/*
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient;

import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLPeerUnverifiedException;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.URL;
import java.security.cert.Certificate;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;
import org.json.*;
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

    // Default observations URL
    private static String observationsUrl = stagingObservationsEndpoint;

    // Class specific
    private static String clientId = null;
    private static String clientSecret = null;
    private static String deviceId = null;
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

        // Set test observations endpoint if using the test environment
        if (sfUseTestEnvironment)
        {
            observationsUrl = testObservationsEndpoint;
        }
    }


    public void sendObservation(String observation)
    {
        // Get token
        TokenClient tc = new TokenClient(useTestEnvironment);
        String token;
        try
        {
            token = tc.getApiToken(clientId, clientSecret);
        }
        catch (Exception excptn)
        {
            log.error("Failed to acquire API token: {}", excptn.getMessage());
            log.info("Skipping send of observation to Starfish");
            return;
        }

        // Build observations payload
        String payload = buildPayload(observation);

        // Send observations
        try
        {
            sendObservations(deviceId, payload, token);
        }
        catch (Exception excptn)
        {
            log.error("Exception in sendObservations: {}", excptn.getMessage());
            log.error("Stack Trace sendObservations: {}", excptn.getCause());
            log.info("Skipping send of observation to Starfish");
        }
    }


    private String buildPayload(String observation)
    {
        String[] parts = observation.split(",", 3);

        Long ts = new Long(parts[0]);
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'");
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
        Date dt = new Date(ts);
        String timestamp = sdf.format(dt);
        log.debug("timestamp: {}", timestamp);

        // Convert the observation
        JSONObject joObservation = new JSONObject();
        joObservation.put("timestamp", timestamp);
        joObservation.put("temperature", parts[1]);

        JSONArray joaObservations = new JSONArray();
        joaObservations.put(joObservation);

        // Build payload
        JSONObject jo = new JSONObject();
        jo.put("observations", joaObservations);

        String json = jo.toString();
        log.info("Starfish payload: {}", json);

        return json;
    }


    // HTTP POST request
    private void sendObservations(String deviceId, String payload, String token) throws Exception
    {
        String fullUrl = observationsUrl + "/" + deviceId + "/observations";

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
            log.debug("Starfish observations response: {}", response);
        }
        else
        {
            log.debug("Starfish Observations Status: {} ",httpStatus);
        }
        con.disconnect();
    }


    private void print_https_cert(HttpsURLConnection con)
    {
        if (con!=null)
        {
            try {

                System.out.println("Response Code : " + con.getResponseCode());
                System.out.println("Cipher Suite : " + con.getCipherSuite());
                System.out.println("\n");

                Certificate[] certs = con.getServerCertificates();
                for(Certificate cert : certs)
                {
                    System.out.println("Cert Type : " + cert.getType());
                    System.out.println("Cert Hash Code : " + cert.hashCode());
                    System.out.println("Cert Public Key Algorithm : "
                            + cert.getPublicKey().getAlgorithm());
                    System.out.println("Cert Public Key Format : "
                            + cert.getPublicKey().getFormat());
                    System.out.println("\n");
                }

            } catch (SSLPeerUnverifiedException e) {
                e.printStackTrace();
            } catch (IOException e){
                e.printStackTrace();
            }
        }
    }
}
