/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient;

/**
 * CoapDevice. Contains the details of a device (the HDK). It acts as a partial cache. Note, to be used with upcoming features.
 *
 * @author Bob Luben
 */
public class CoapDevice
{
    private long maxAge;


    public long getMaxAge() {
        return maxAge;
    }
    public void setMaxAge(long newMaxAge) {
        maxAge =  newMaxAge;
    }
}
