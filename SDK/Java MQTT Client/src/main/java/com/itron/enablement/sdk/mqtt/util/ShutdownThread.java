package com.itron.enablement.sdk.mqtt.util;


public class ShutdownThread extends Thread
{
    private IShutdownThreadParent shutdownThreadParent;

    public ShutdownThread(IShutdownThreadParent shutdownThreadParent)
    {
        this.shutdownThreadParent = shutdownThreadParent;
    }

    @Override
    public void run()
    {
        this.shutdownThreadParent.shutdown();
    }
}
