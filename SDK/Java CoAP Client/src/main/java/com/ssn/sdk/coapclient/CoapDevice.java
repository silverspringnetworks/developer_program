/**
 * Copyright (c) 2017, Silver Spring Networks, Inc.
 * All rights reserved
 */
package com.ssn.sdk.coapclient;

/**
 * Created by bob on 3/21/2017.
 */
public class CoapDevice
{
    private long maxAge = 0;


    /**
     * Returns the CoAP method to be used with request
     * @return the CoAP method to be used with request
     */
    public long getMaxAge() {
        return maxAge;
    }

    public void setMaxAge(long newMaxAge) {
        maxAge =  newMaxAge;
    }
}
