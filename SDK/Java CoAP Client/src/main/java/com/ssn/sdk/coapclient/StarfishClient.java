/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient;

import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLPeerUnverifiedException;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
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

    private static String tokenUrl = "https://poc.api.ssniot.cloud/api/tokens"; //Note: revised on 20170215
    private static String observationsUrl = "https://poc.api.dev.ssniot.cloud/api/solutions/lpoc/devices";

    // Dev Specific
    private static String clientId = "52a19a7012db269e412f6596efcdc0e808cc0b29bd98241";
    private static String clientSecret = "850136671";

    private static String apiToken = null;
    private static String lnicMacAddress = null;
    private static String apMacAddress = null;


    /**
     *  Initializes an instance of {@link SdkCoapObserver}.
     *
     */
    public StarfishClient(String sfClientId, String sfClientSecret )
    {
        clientId = sfClientId;
        clientSecret = sfClientSecret;
    }


    public void sendObservation(String observation, String deviceId)
    {
        // Get token
        try
        {
            getApiToken();
        }
        catch (Exception excptn)
        {
            log.error("Exception in getApiToken: {}", excptn.getMessage());
            log.info("Skipping send of observation to Starfish");
            return;
        }

        // Build observations payload
        String payload = buildPayload(observation);

        // Send observations
        try
        {
            sendObservations(deviceId, payload);
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
        log.debug("Starfish payload: {}", json);

        return json;
    }


    private void getApiToken() throws Exception
    {
        URL obj = new URL(tokenUrl);
        HttpsURLConnection con = (HttpsURLConnection) obj.openConnection();

        // To dump info on the cert
        //print_https_cert(con);

        // Setup request
        con.setRequestMethod("POST");
        con.setRequestProperty("Content-Type", "application/json");
        con.setRequestProperty("Accept", "application/json");

        // Dev or POC?
        String requestPayload;
        requestPayload = "{ \"clientId\": \"" + clientId + "\", \"clientSecret\": \"" + clientSecret + "\" }";

        // Send request
        con.setDoOutput(true);
        DataOutputStream wr = new DataOutputStream(con.getOutputStream());
        wr.writeBytes(requestPayload);
        wr.flush();
        wr.close();
        //System.out.println("***TRACE: Sending 'POST' request to URL : " + tokenUrl);
        //System.out.println("***TRACE: Post payload : " + requestPayload);

        // Read response
        int httpStatus = con.getResponseCode();
        log.debug("Starfish tokens Status: {} ", httpStatus);
        if (httpStatus != 200 && httpStatus != 201)
        {
            log.error("Starfish tokens call failed: {}", httpStatus);
        }

        BufferedReader in = new BufferedReader(new InputStreamReader(con.getInputStream()));
        String inputLine;
        StringBuffer response = new StringBuffer();

        while ((inputLine = in.readLine()) != null)
        {
            response.append(inputLine);
        }
        in.close();
        con.disconnect();

        // Capture token
        JSONObject jo = new JSONObject(response.toString());
        apiToken = jo.getString("accessToken");
        log.debug("Starfish API token: {}", apiToken);
    }


    // HTTP POST request
    private void sendObservations(String deviceId, String payload) throws Exception
    {
        String fullUrl = observationsUrl + "/" + deviceId + "/observations";
        URL obj = new URL(fullUrl);
        HttpsURLConnection con = (HttpsURLConnection) obj.openConnection();

        // Setup request
        con.setRequestMethod("POST");
        con.setRequestProperty("Content-Type", "application/json");
        con.setRequestProperty("Accept", "application/json");
        con.setRequestProperty("Authorization", apiToken);

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
