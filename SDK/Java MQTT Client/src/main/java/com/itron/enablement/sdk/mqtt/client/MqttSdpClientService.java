package com.itron.enablement.sdk.mqtt.client;

import com.itron.enablement.sdk.mqtt.util.IShutdownThreadParent;
import com.itron.enablement.sdk.mqtt.util.MqttSdpConfiguration;
import com.itron.enablement.sdk.mqtt.util.MqttSdpConnectOptions;
import com.itron.enablement.sdk.mqtt.util.ShutdownThread;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;


/**
 * MqttSdpClientService
 *
 * The driver.
 * Limitations
 *   - Can handle 10000 devices max (see device cache)
 *   - Client must be restarted to pick new devices added to the Starfish device repo
 *
 */
public class MqttSdpClientService implements IShutdownThreadParent
{
    private static final Logger logger = LogManager.getLogger(MqttSdpClientService.class.getName());

    private MqttSdpClient mqttBridge;
    private MqttSdpConfiguration bridgeConfiguration;
    // Set to true on a SIGINT
    private boolean done = false;
    // Set to true when connection to MQTT broker is lost
    private boolean lostconnection = false;


    public void MqttSdpClientService()
    {
        ShutdownThread shutdownThread;

        // Instantiate a new ShutdownThread instance and invoke the addShutdownHook method
        shutdownThread = new ShutdownThread(this);
        Runtime.getRuntime().addShutdownHook(shutdownThread);
    }


    public void start()
    {
        logger.debug("Starting up MQTT Client Service");

        // Setup configuration
        bridgeConfiguration = new MqttSdpConfiguration();
        bridgeConfiguration.init();
        bridgeConfiguration.printToLog();


        // Build the device cache if configured
        if (bridgeConfiguration.getSdp_get_device_info() == 1)
        {
            // Initialize the device cache
            boolean succeeded = MqttSdpDeviceCache.initializeDeviceCache(bridgeConfiguration);
            if (!succeeded)
            {
                logger.error("Failed to initialize Device Cache - not starting MQTT Client Service!");
                return;
            }
        }
        else
        {
            logger.info("Device Cache NOT initialized!");
        }

        // Get connection options
        MqttSdpConnectOptions connectionOptions = new MqttSdpConnectOptions(bridgeConfiguration);

        // Connect to the broker
        mqttBridge = new MqttSdpClient(bridgeConfiguration, connectionOptions, this);
        boolean connected = mqttBridge.connectToMqttBroker(bridgeConfiguration.getClientId());
        if (connected)
        {
            mqttBridge.subscribeToMqttBroker(bridgeConfiguration.getBroker_topic(), bridgeConfiguration.getBroker_QoS());
        }
        else
        {
            logger.error("Could not get a connection to the broker. Broker may not be running. Giving up!");
            System.exit(2);
        }
    }


    public void loop()
    {
        while (!isDone())
        {
            // Spin lock with delay
            try
            {
                Thread.sleep(bridgeConfiguration.getIdle_loop_delay());
            }
            catch (Exception excpn)
            {
                setDone(true);
            }
            // Check for lost connection
            if (isLostconnection())
            {
                logger.error("Connection to Broker lost. Resetting Connection!");
                mqttBridge.shutdownMqttBrokerConnection();
                boolean connected = mqttBridge.connectToMqttBroker(bridgeConfiguration.getClientId());
                if (connected)
                {
                    mqttBridge.subscribeToMqttBroker(bridgeConfiguration.getBroker_topic(), bridgeConfiguration.getBroker_QoS());
                    setLostconnection(false);
                }
            }
        }
    }


    public void stop()
    {
        mqttBridge.shutdownMqttBrokerConnection();
        System.out.println("Stopped MQTT Bridge");
    }


    @Override
    public void shutdown()
    {
        logger.info("SIGINT - Handling JVM shutdown!");
        setDone(true);
        stop();
    }


    public static void main( String[] args )
    {
        logger.info("MQTT Client Service!");

        MqttSdpClientService mcs = new MqttSdpClientService();

        // Start the service
        mcs.start();

        // Loop forever while the client processes messages
        mcs.loop();

        // If we get here, stop the service
        mcs.stop();
    }


    public boolean isDone() {
        return done;
    }

    public void setDone(boolean done) {
        this.done = done;
    }

    public boolean isLostconnection() {
        return lostconnection;
    }

    public void setLostconnection(boolean lostconnection) {
        this.lostconnection = lostconnection;
    }
}
