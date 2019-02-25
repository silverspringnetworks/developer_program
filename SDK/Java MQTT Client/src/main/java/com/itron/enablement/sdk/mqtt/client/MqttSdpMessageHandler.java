package com.itron.enablement.sdk.mqtt.client;

import com.itron.enablement.sdk.mqtt.sdp.TempHandler;
import com.itron.enablement.sdk.mqtt.sdp.LnicHandler;
import com.itron.enablement.sdk.mqtt.sdp.Ch4Handler;
import com.itron.enablement.sdk.mqtt.util.MqttSdpConfiguration;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttMessage;

/**
 * Created by bluben on 1/8/18.
 */
public class MqttSdpMessageHandler implements MqttCallback
{
    private static final Logger logger = LogManager.getLogger(MqttSdpMessageHandler.class.getName());

    private MqttSdpClientService clientService;
    private MqttSdpConfiguration bridgeConfiguration;


    public MqttSdpMessageHandler(MqttSdpConfiguration bridgeConfiguration, MqttSdpClientService service)
    {
        this.bridgeConfiguration = bridgeConfiguration;
        this.clientService = service;
    }


    /**
     * connectionLost
     * This callback is invoked upon losing the MQTT connection.
     */
    @Override
    public void connectionLost(Throwable reason)
    {
        // Ignore spurious connectonLost events. Keeps the connection reset from cascading.
        if ( !getClientService().isLostconnection())
        {
            logger.error("Connection to Broker lost: ".concat(reason.getMessage()));

            // Mark the MQTT connection as lost in the client service so it can reset
            getClientService().setLostconnection(true);
        }
    }


    /**
     * deliveryComplete
     * This callback is invoked when a message published by this client
     * is successfully received by the broker.
     */
    @Override
    public void deliveryComplete(IMqttDeliveryToken token)
    {
        logger.error("Received delivery notification - expected none!");
    }


    /**
     * messageArrived
     * This callback is invoked when a message is received on a subscribed topic.
     */
    @Override
    public void messageArrived(String topic, MqttMessage message)
    {
        logger.debug("Message on Topic: ".concat(topic));

        // Lookup the deviceId associated with the nic_macID provided in the topic
        // Expect a topic with this form: "<tenantId>/alerts/<macId>/<sensor type>"
        String[] topicParts = topic.split("/");
        String nic_macID = topicParts[2];
        String sensorType = topicParts[3];
        logger.debug("Topic nic_macID: {}", nic_macID);
        logger.debug("Topic sensorType: {}", sensorType);


        // Add a case to this switch to add a new sensor payload.
        switch (sensorType)
        {
            case "temp":
            {
                // Grab the device id.
                String deviceId = MqttSdpDeviceCache.getDeviceCache().get(nic_macID);
                logger.debug("Device Id: {}", deviceId);
                // Transform wfci observation and send to Starfish Data Platform.
                sendTempData2Sdp(message, deviceId);
                break;
            }
            case "ncmth1":
            {
                // Grab the device id.
                String deviceId = MqttSdpDeviceCache.getDeviceCache().get(nic_macID);
                logger.debug("Device Id: {}", deviceId);
                // Transform observation and send to Starfish Data Platform.
                sendCh4Data2Sdp(message, deviceId, nic_macID);
                break;
            }
            case "lnic":
            {
                // Grab the device id.
                String deviceId = MqttSdpDeviceCache.getDeviceCache().get(nic_macID);
                logger.debug("Device Id: {}", deviceId);

                // Transform observation and send to Starfish Data Platform.
                sendLnicData2Sdp(message, nic_macID, deviceId);
                break;
            }
            default:
            {
                logger.info("Unrecognized Message on Topic: ".concat(topic));
                break;
            }
        }
    }


    private void sendTempData2Sdp(MqttMessage message, String deviceId)
    {
        // Transform the payload and send it to SDP
        TempHandler temph = new TempHandler(getBridgeConfiguration());
        String payloadTransformer = "com.itron.enablement.sdk.mqtt.payload.TempPayloadTransformer";
        byte[] payloadAsByteArray = message.getPayload();
        temph.sendObservation(payloadAsByteArray, payloadTransformer, deviceId);
    }


    private void sendLnicData2Sdp(MqttMessage message, String macId, String deviceId)
    {
        // Transform the payload and send it to SDP
        LnicHandler lnich = new LnicHandler(getBridgeConfiguration());
        String payloadTransformer = "com.itron.enablement.sdk.mqtt.payload.LnicPayloadTransformer";
        byte[] payloadAsByteArray = message.getPayload();
        lnich.sendObservation(payloadAsByteArray, payloadTransformer, macId, deviceId);
    }


    private void sendCh4Data2Sdp(MqttMessage message, String deviceId, String nic_macID)
    {
        // Transform the payload and send it to SDP
        Ch4Handler ch4h = new Ch4Handler(getBridgeConfiguration());
        String payloadTransformer = "com.itron.enablement.sdk.mqtt.payload.Ch4PayloadTransformer";
        byte[] payloadAsByteArray = message.getPayload();
        ch4h.sendObservation(payloadAsByteArray, payloadTransformer, deviceId, nic_macID);
    }


    private MqttSdpConfiguration getBridgeConfiguration() {
        return bridgeConfiguration;
    }

    private MqttSdpClientService getClientService() {
        return clientService;
    }
}
