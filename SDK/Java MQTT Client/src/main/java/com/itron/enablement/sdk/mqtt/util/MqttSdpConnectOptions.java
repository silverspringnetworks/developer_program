package com.itron.enablement.sdk.mqtt.util;

import com.itron.enablement.sdk.mqtt.client.MqttSdpClientService;
import com.itron.enablement.sdk.mqtt.sdp.TokenClient;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;

public class MqttSdpConnectOptions
{
    private static final Logger logger = LogManager.getLogger(MqttSdpConnectOptions.class.getName());

    private MqttSdpConfiguration bridgeConfiguration;
    private MqttConnectOptions mqttConnectionOptions;


    public MqttSdpConnectOptions(MqttSdpConfiguration bridgeConfig)
    {
        this.bridgeConfiguration = bridgeConfig;
    }


    /**
     *
     * setupConnectionOptions
     *
     */
    public void setupConnectionOptions()
    {
        setMqttConnectionOptions(new MqttConnectOptions());
        getMqttConnectionOptions().setCleanSession(true);
        getMqttConnectionOptions().setAutomaticReconnect(true);
        getMqttConnectionOptions().setKeepAliveInterval(getBridgeConfiguration().getKeep_alive_interval());

        // If using CAAS authentication set the user name and password.
        if (getBridgeConfiguration().getBroker_use_token_auth() == 0)
        {
            getMqttConnectionOptions().setUserName(getBridgeConfiguration().getBroker_username());
            getMqttConnectionOptions().setPassword(getBridgeConfiguration().getBroker_password().toCharArray());
        }

        // If using token authentication get a Starfish token and set it in the password field.
        if (getBridgeConfiguration().getBroker_use_token_auth() == 1)
        {
            getMqttConnectionOptions().setUserName("notused");

            // Get a Starfish token
            String token;
            PayloadUtilities pu = new PayloadUtilities();
            token = pu.getStarfishToken(getBridgeConfiguration());
            if (token != null)
            {
                getMqttConnectionOptions().setPassword(token.toCharArray());
            }
            else
            {
                logger.error("No token - can't connect to broker - exiting!");
                System.exit(3);
            }
        }
    }


    public MqttSdpConfiguration getBridgeConfiguration() {
        return bridgeConfiguration;
    }

    public void setBridgeConfiguration(MqttSdpConfiguration bridgeConfiguration) {
        this.bridgeConfiguration = bridgeConfiguration;
    }

    public MqttConnectOptions getMqttConnectionOptions() {
        return mqttConnectionOptions;
    }

    public void setMqttConnectionOptions(MqttConnectOptions mqttConnectionOptions) {
        this.mqttConnectionOptions = mqttConnectionOptions;
    }
}
