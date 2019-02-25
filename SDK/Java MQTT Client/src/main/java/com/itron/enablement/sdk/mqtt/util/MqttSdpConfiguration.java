package com.itron.enablement.sdk.mqtt.util;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;


public class MqttSdpConfiguration
{
    private static final Logger logger = LogManager.getLogger(MqttSdpConfiguration.class.getName());

    private String  broker_protocol;
    private String  broker_port;
    private String  broker_host;
    private String  broker_url;
    private Integer broker_use_token_auth;
    private String  broker_username;
    private String  broker_password;
    private String  broker_topic;
    private String  clientId;
    private int     broker_QoS;
    private String  broker_tenant_id;
    private String  sdp_clientId;
    private String  sdp_clientSecret;
    private String  sdp_token_endpoint;
    private String  sdp_observations_endpoint;
    private int     sdp_get_device_info;
    private int     max_connection_attempts;
    private int     connection_delay;
    private int     idle_loop_delay;
    private int     keep_alive_interval;


    // Load up properties form config.properties (via ConfigProperties)
    public void init()
    {
        this.setBroker_protocol(ConfigProperties.getProperty("MQTTSDPBRIDGE_BROKER_PROTOCOL", "ssl"));
        this.setBroker_port(ConfigProperties.getProperty("MQTTSDPBRIDGE_BROKER_PORT", "8883"));
        this.setBroker_host(ConfigProperties.getProperty("MQTTSDPBRIDGE_BROKER_HOST", ""));

        String protocol = this.getBroker_protocol();
        String port = this.getBroker_port();
        String host = this.getBroker_host();
        String url = protocol.concat("://").concat(host).concat(":").concat(port);
        this.setBroker_url(url);

        this.setBroker_use_token_auth(new Integer(ConfigProperties.getProperty("MQTTSDPBRIDGE_USE_TOKEN_AUTH", "1")));
        this.setBroker_tenant_id(ConfigProperties.getProperty("MQTTSDPBRIDGE_TENANT_ID", "ssni"));

        this.setBroker_username(ConfigProperties.getProperty("MQTTSDPBRIDGE_USERNAME", ""));
        this.setBroker_password(ConfigProperties.getProperty("MQTTSDPBRIDGE_PASSWORD", ""));

        this.setBroker_QoS(new Integer(ConfigProperties.getProperty("MQTTSDPBRIDGE_QOS", "0")));
        this.setClientId(ConfigProperties.getProperty("MQTTSDPBRIDGE_CLIENT_ID", ""));
        this.setBroker_topic(ConfigProperties.getProperty("MQTTSDPBRIDGE_TOPIC", ""));
        this.setSdp_clientId(ConfigProperties.getProperty("MQTTSDPBRIDGE_SDP_CLIENT_ID", ""));
        this.setSdp_clientSecret(ConfigProperties.getProperty("MQTTSDPBRIDGE_SDP_CLIENT_SECRET", ""));

        // Starfish DP related
        // Note: Starfish Test token endpoint: https://poc.api.ssniot.cloud/api/tokens
        // Note: Starfish Staging token endpoint: https://api.data-platform.developer.ssni.com/api/tokens
        this.setSdp_token_endpoint(ConfigProperties.getProperty("MQTTSDPBRIDGE_SDP_TOKEN_ENDPOINT", ""));
        // Note: Starfish Test observations endpoint: https://poc.api.dev.ssniot.cloud/api/solutions/sandbox/devices
        this.setSdp_observations_endpoint(ConfigProperties.getProperty("MQTTSDPBRIDGE_SDP_OBSERVATION_ENDPOINT", ""));
        this.setSdp_get_device_info(new Integer(ConfigProperties.getProperty("MQTTSDPBRIDGE_SDP_GET_DEVICE_INFO", "1")));

        // Connection retry and idle loop related
        this.setMax_connection_attempts(new Integer(ConfigProperties.getProperty("MQTTSDPBRIDGE_CONNECTION_ATTEMPTS", "1440")));
        this.setConnection_delay(new Integer(ConfigProperties.getProperty("MQTTSDPBRIDGE_CONNECTION_DELAY", "60000")));
        this.setIdle_loop_delay(new Integer(ConfigProperties.getProperty("MQTTSDPBRIDGE_IDLE_LOOP_DELAY", "100")));
        this.setKeep_alive_interval(new Integer(ConfigProperties.getProperty("MQTTSDPBRIDGE_KEEP_ALIVE_INTERVAL", "3")));
    }


    public void printToLog()
    {
        logger.debug("Start Property Dump");

        logger.debug("Broker URL: ".concat(this.getBroker_url()));
        logger.debug("Use Token Auth: ".concat(this.getBroker_use_token_auth().toString()));
        logger.debug("Tenant Id: ".concat(this.getBroker_tenant_id()));
        logger.debug("Username: ".concat(this.getBroker_username()));
        logger.debug("Password: ".concat(this.getBroker_password()));
        logger.debug("Broker QOS: ".concat(this.getBroker_QoS().toString()));
        logger.debug("SDP ClientId: ".concat(this.getClientId()));
        logger.debug("Broker Topic: ".concat(this.getBroker_topic()));
        logger.debug("SDP ClientId: ".concat(this.getSdp_clientId()));
        logger.debug("SDP ClientSecret: ".concat(this.getSdp_clientSecret()));
        logger.debug("SDP Token Endpoint: ".concat(this.getSdp_token_endpoint()));
        logger.debug("SDP Observation Endpoint: ".concat(this.getSdp_observations_endpoint()));
        logger.debug("SLDP Get Device Info: ".concat(this.getSdp_get_device_info().toString()));
        logger.debug("Max Connection Attempts: ".concat(this.getMax_connection_attempts().toString()));
        logger.debug("Connection Delay: ".concat(this.getConnection_delay().toString()));
        logger.debug("Idle Loop Delay: ".concat(this.getIdle_loop_delay().toString()));
        logger.debug("Keep Alive Interval: ".concat(this.getKeep_alive_interval().toString()));

        logger.debug("End Property Dump");
    }


    public String getBroker_url() {
        return broker_url;
    }

    public void setBroker_url(String broker_url) {
        this.broker_url = broker_url;
    }

    public String getBroker_username() {
        return broker_username;
    }

    public void setBroker_username(String broker_username) {
        this.broker_username = broker_username;
    }

    public String getBroker_password() {
        return broker_password;
    }

    public void setBroker_password(String broker_password) {
        this.broker_password = broker_password;
    }

    public String getBroker_topic() {
        return broker_topic;
    }

    public void setBroker_topic(String broker_topic) {
        this.broker_topic = broker_topic;
    }

    public String getClientId() {
        return clientId;
    }

    public void setClientId(String clientId) {
        this.clientId = clientId;
    }

    public Integer getBroker_QoS() {
        return broker_QoS;
    }

    public void setBroker_QoS(Integer broker_QoS) {
       this.broker_QoS = broker_QoS;
    }

    public String getSdp_clientSecret() {
        return sdp_clientSecret;
    }

    public void setSdp_clientSecret(String sdp_clientSecret) {
        this.sdp_clientSecret = sdp_clientSecret;
    }

    public String getSdp_token_endpoint() {
        return sdp_token_endpoint;
    }

    public void setSdp_token_endpoint(String sdp_token_endpoint) {
        this.sdp_token_endpoint = sdp_token_endpoint;
    }

    public String getSdp_observations_endpoint() {
        return sdp_observations_endpoint;
    }

    public void setSdp_observations_endpoint(String sdp_observations_endpoint) {
        this.sdp_observations_endpoint = sdp_observations_endpoint;
    }

    public String getSdp_clientId() {
        return sdp_clientId;
    }

    public void setSdp_clientId(String sdp_clientId) {
        this.sdp_clientId = sdp_clientId;
    }

    public String getBroker_protocol() {
        return broker_protocol;
    }

    public void setBroker_protocol(String broker_protocol) {
        this.broker_protocol = broker_protocol;
    }

    public String getBroker_port() {
        return broker_port;
    }

    public void setBroker_port(String broker_port) {
        this.broker_port = broker_port;
    }

    public String getBroker_host() {
        return broker_host;
    }

    public void setBroker_host(String broker_host) {
        this.broker_host = broker_host;
    }

    public Integer getMax_connection_attempts() {
        return max_connection_attempts;
    }

    public void setMax_connection_attempts(Integer max_connection_attempts) {
        this.max_connection_attempts = max_connection_attempts;
    }

    public Integer getConnection_delay() {
        return connection_delay;
    }

    public void setConnection_delay(Integer connection_delay) {
        this.connection_delay = connection_delay;
    }

    public Integer getIdle_loop_delay() {
        return idle_loop_delay;
    }

    public void setIdle_loop_delay(Integer idle_loop_delay) {
        this.idle_loop_delay = idle_loop_delay;
    }

    public Integer getSdp_get_device_info() {
        return sdp_get_device_info;
    }

    public void setSdp_get_device_info(Integer sdp_get_device_info) {
        this.sdp_get_device_info = sdp_get_device_info;
    }

    public Integer getBroker_use_token_auth() {
        return broker_use_token_auth;
    }

    public void setBroker_use_token_auth(Integer broker_use_token_auth) {
        this.broker_use_token_auth = broker_use_token_auth;
    }

    public String getBroker_tenant_id() {
        return broker_tenant_id;
    }

    public void setBroker_tenant_id(String broker_tenant_id) {
        this.broker_tenant_id = broker_tenant_id;
    }

    public Integer getKeep_alive_interval() {
        return keep_alive_interval;
    }

    public void setKeep_alive_interval(Integer keep_alive_interval) {
        this.keep_alive_interval = keep_alive_interval;
    }
}
