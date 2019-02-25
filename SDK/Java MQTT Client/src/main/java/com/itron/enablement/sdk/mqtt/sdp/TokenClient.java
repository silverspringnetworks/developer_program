package com.itron.enablement.sdk.mqtt.sdp;


import com.itron.enablement.sdk.mqtt.util.MqttSdpConfiguration;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.json.JSONObject;

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
    private static final Logger log = LogManager.getLogger(TokenClient.class.getName());

    private MqttSdpConfiguration bridgeConfiguration;


    public TokenClient(MqttSdpConfiguration bridgeConfig)
    {
        this.bridgeConfiguration = bridgeConfig;
    }


    public String getApiToken() throws Exception
    {
        String tokenUrl = this.getBridgeConfiguration().getSdp_token_endpoint();
        String clientId = this.getBridgeConfiguration().getSdp_clientId();
        String clientSecret = this.getBridgeConfiguration().getSdp_clientSecret();

        URL obj = new URL(tokenUrl);
        log.trace("Starfish tokens URL: {}", tokenUrl);
        HttpsURLConnection con = (HttpsURLConnection) obj.openConnection();

        // Setup request
        con.setRequestMethod("POST");
        con.setRequestProperty("Content-Type", "application/json");
        con.setRequestProperty("Accept", "application/json");

        // Hand craft the JSON. Don't do this in general. It's easy.
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
            con.disconnect();
            return null;
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
        log.trace("Starfish API token: {}", apiToken);
        return apiToken;
    }


    public MqttSdpConfiguration getBridgeConfiguration() {
        return bridgeConfiguration;
    }

    public void setBridgeConfiguration(MqttSdpConfiguration bridgeConfiguration) {
        this.bridgeConfiguration = bridgeConfiguration;
    }
}