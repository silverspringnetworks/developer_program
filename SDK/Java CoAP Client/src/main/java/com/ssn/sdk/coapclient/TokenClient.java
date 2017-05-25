/*
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient;

import org.json.JSONObject;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.net.ssl.HttpsURLConnection;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.net.URL;

/**
 *
 * @author bluben
 */
public class TokenClient
{
    private Logger log = LoggerFactory.getLogger(this.getClass().getName());

    // Starfish Staging and Production enviornment tokens endpoint
    private static final String stagingTokensEndpoint = "https://api.data-platform.developer.ssni.com/api/tokens";

    // Starfish Test enviornment tokens endpoint
    private static final String testTokensEndpoint = "https://poc.api.ssniot.cloud/api/tokens";

    // Default tokenUrl
    private static String tokenUrl = stagingTokensEndpoint;


    /**
     *  Initializes an instance of {@link TokenClient}.
     *
     */
    public TokenClient(boolean useTestEnvironment)
    {
        // Set test tokens endpoint if using the test environment
        if (useTestEnvironment)
        {
            tokenUrl = testTokensEndpoint;
        }
    }



    public String getApiToken(String clientId, String clientSecret) throws Exception
    {
        URL obj = new URL(tokenUrl);
        log.debug("Starfish tokens URL: {}", tokenUrl);
        HttpsURLConnection con = (HttpsURLConnection) obj.openConnection();

        // Setup request
        con.setRequestMethod("POST");
        con.setRequestProperty("Content-Type", "application/json");
        con.setRequestProperty("Accept", "application/json");

        // Hand craft the JSON
        //TODO: Don't do this
        String requestPayload = "{ \"clientId\": \"" + clientId + "\", \"clientSecret\": \"" + clientSecret + "\" }";

        // Send request
        con.setDoOutput(true);
        DataOutputStream wr = new DataOutputStream(con.getOutputStream());
        wr.writeBytes(requestPayload);
        wr.flush();
        wr.close();

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
        String apiToken = jo.getString("accessToken");
        log.debug("Starfish API token: {}", apiToken);
        return apiToken;
    }
}
