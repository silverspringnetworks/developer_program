package com.itron.enablement.sdk.mqtt.util;


import java.io.File;
import java.io.FileInputStream;
import java.util.Properties;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;


public class ConfigProperties
{
    private static final Logger logger = LogManager.getLogger(ConfigProperties.class.getName());

    private static final ConfigProperties configProperties = new ConfigProperties();
    private static Properties properties;


    private ConfigProperties()
    {
        try {
            String propFilePath = System.getProperty("config.properties");
            logger.info("Loading properties from {}", propFilePath);
            FileInputStream propFile =
                    new FileInputStream(getConfigurationFile(propFilePath));
            properties = new Properties(System.getProperties());
            properties.load(propFile);
        } catch (Exception e) {
            logger.error("Failed to load properties file", e);
            throw new RuntimeException("Failed to load properties file",e) ;
        }
    }


    public static ConfigProperties getInstance()
    {
        return configProperties ;
    }

    public static String getProperty(String key, String defaultValue)
    {
        String prop = getProperties().getProperty(key, defaultValue);
        return prop;
    }

    private static File getConfigurationFile(String fileName) {
        if ( fileName != null ) {
            File file = new File(fileName);
            if ( file.canRead() ) {
                return file;
            }
        }
        return null;
    }

    private static Properties getProperties() {
        return properties;
    }

    private static void setProperties(Properties properties) {
        ConfigProperties.properties = properties;
    }
}
