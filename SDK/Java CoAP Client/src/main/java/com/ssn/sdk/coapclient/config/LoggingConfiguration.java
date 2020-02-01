/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient.config;

import org.apache.log4j.xml.DOMConfigurator;
import java.net.URL;


/**
 * Logging support class
 *
 * @author Bob Luben
 */
public abstract class LoggingConfiguration
{
    /**
     * Configure a logging scheme.
     *
     * Refer to {@link org.apache.log4j.Level#INFO}
     * Supplied configuration provides for all loggers with console output.
     */
    public static void configureLogging()
    {
        URL url = LoggingConfiguration.class.getClassLoader().getResource("log4j.default.xml");
        DOMConfigurator.configure(url);
    }
}
