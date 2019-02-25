package com.itron.enablement.sdk.mqtt.client;

import com.itron.enablement.sdk.mqtt.util.MqttSdpConfiguration;
import com.itron.enablement.sdk.mqtt.util.MqttSdpConnectOptions;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.eclipse.paho.client.mqttv3.*;


/**
 * Created by bluben on 1/2/18.
 */
public class MqttSdpClient
{
    private static final Logger logger = LogManager.getLogger(MqttSdpClient.class.getName());

    private MqttAsyncClient myClient;
    private MqttSdpConfiguration bridgeConfiguration;
    private MqttSdpConnectOptions connectOptions;
    private MqttSdpClientService clientService;


    public MqttSdpClient(MqttSdpConfiguration bridgeConfig, MqttSdpConnectOptions connectOptions, MqttSdpClientService service)
    {
        this.bridgeConfiguration = bridgeConfig;
        this.connectOptions = connectOptions;
        this.clientService = service;
    }


    /**
     *
     * connectToMqttBroker
     *
     */
    public boolean connectToMqttBroker(String clientId)
    {
        MqttSdpMessageHandler messageHandler;
        IMqttToken asyncToken;

        // Get our callback handler
        messageHandler = new MqttSdpMessageHandler(getBridgeConfiguration(), getClientService());

        // Connect to Broker
        int maxConnectAttempts = getBridgeConfiguration().getMax_connection_attempts();
        int delayBetweenAttempts = getBridgeConfiguration().getConnection_delay();
        boolean connected = true;
        for (int indx=0 ; indx < maxConnectAttempts ; indx++)
        {
            try
            {
                logger.info("Attempting to connect to Broker");

                this.getConnectOptions().setupConnectionOptions();

                myClient = new MqttAsyncClient(getBridgeConfiguration().getBroker_url(), clientId);
                myClient.setCallback(messageHandler);
                asyncToken = myClient.connect(getConnectOptions().getMqttConnectionOptions());
                asyncToken.waitForCompletion();
                connected = true;
                break;
            }
            catch (MqttException excpn)
            {
                logger.warn("Failed to connect to broker: Exception: {}", excpn.getMessage());
                connected = false;
            }
            // Delay between attemtps
            try
            {
                Thread.sleep(delayBetweenAttempts);
            }
            catch (Exception excpn)
            {
                System.exit(-1);
            }
        }

        if (connected)
        {
            logger.info("Connected to Broker: ".concat(getBridgeConfiguration().getBroker_url()));
            return true;
        }
        else
        {
            logger.error("Failed ongoing connection attempts to Broker: ".concat(getBridgeConfiguration().getBroker_url()));
            return false;
        }
    }


    /**
     *
     * subscribeToMqttBroker
     *
     */
    public void subscribeToMqttBroker(String topic, int topicQoS)
    {
        logger.debug("Subscribing to Topic");

        // Subscribe to topic
        IMqttToken asyncToken;
        try
        {
            asyncToken = myClient.subscribe(topic, topicQoS);
            asyncToken.waitForCompletion();
        }
        catch (Exception excpn)
        {
            logger.warn("Failed to subscribe to topic: Exception: {}", excpn.getMessage());
            return;
        }
        logger.info("Subscribed to Topic: ".concat(topic));
    }


    /**
     *
     * shutdownMqttBrokerConnection
     *
     */
    public void shutdownMqttBrokerConnection()
    {
        logger.debug("Disconnect and close connection with Broker!");
        try {
            myClient.disconnect();
            myClient.close();
        }
        catch (Exception excpn)
        {
            logger.warn("Exception occurred while closing Broker connection: {}", excpn.getMessage());
        }
        logger.info("Disconnected from Broker!");
    }


    private MqttSdpConfiguration getBridgeConfiguration() {
        return bridgeConfiguration;
    }

    private MqttSdpConnectOptions getConnectOptions() {
        return connectOptions;
    }

    private MqttSdpClientService getClientService() {
        return clientService;
    }
}
