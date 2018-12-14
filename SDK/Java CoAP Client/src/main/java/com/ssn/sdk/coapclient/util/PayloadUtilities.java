package com.ssn.sdk.coapclient.util;

import co.nstant.in.cbor.CborDecoder;
import co.nstant.in.cbor.CborException;
import co.nstant.in.cbor.model.DataItem;
import co.nstant.in.cbor.model.ByteString;
import co.nstant.in.cbor.model.UnicodeString;
import co.nstant.in.cbor.model.UnsignedInteger;
//import org.apache.logging.log4j.LogManager;
//import org.apache.logging.log4j.Logger;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLPeerUnverifiedException;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.security.cert.Certificate;
import java.util.List;
import java.util.Scanner;

public class PayloadUtilities
{
    //private static final Logger log = LogManager.getLogger(PayloadUtilities.class.getName());
    private Logger log = LoggerFactory.getLogger(this.getClass().getName());


    // Helper to convert byte array to hex string
    public String bytesToHexString(byte[] bytes)
    {
        StringBuilder sb = new StringBuilder();
        for(byte b : bytes)
        {
            sb.append(String.format("%02x", b&0xff));
        }
        return sb.toString();
    }


    // Helper that converts a InputStream to a String
    public String convertStreamToString(java.io.InputStream is)
    {
        Scanner s = new Scanner(is).useDelimiter("\\A");
        return s.hasNext() ? s.next() : "";
    }


    // Helper to strip off an outer CBOR wrapper. Case where "inner" payload is a byte string type.
    public byte[] stripOffCborWrapperByte(byte[] payload) throws CborException
    {
        ByteArrayInputStream bais = new ByteArrayInputStream(payload);
        List<DataItem> dataItems = new CborDecoder(bais).decode();
        co.nstant.in.cbor.model.Map payloadMap = (co.nstant.in.cbor.model.Map) dataItems.get(0);

        ByteString data = (ByteString) payloadMap.get(new UnsignedInteger(1));
        byte[] innerPayload = data.getBytes();

        return innerPayload;
    }


    // Helper to strip off an outer CBOR wrapper. Case where "inner" payload is text type.
    public String stripOffCborWrapperString(byte[] payload) throws CborException
    {
        ByteArrayInputStream bais = new ByteArrayInputStream(payload);
        List<DataItem> dataItems = new CborDecoder(bais).decode();
        co.nstant.in.cbor.model.Map payloadMap = (co.nstant.in.cbor.model.Map) dataItems.get(0);

        UnicodeString innerPayload = (UnicodeString) payloadMap.get(new UnsignedInteger(1));

        return innerPayload.getString();
    }


    public void print_https_cert(HttpsURLConnection con)
    {
        if (con != null)
        {
            try {

                System.out.println("Response Code : " + con.getResponseCode());
                System.out.println("Cipher Suite : " + con.getCipherSuite());
                System.out.println("\n");

                Certificate[] certs = con.getServerCertificates();
                for(Certificate cert : certs)
                {
                    System.out.println("Cert Type : " + cert.getType());
                    System.out.println("Cert Hash Code : " + cert.hashCode());
                    System.out.println("Cert Public Key Algorithm : "
                            + cert.getPublicKey().getAlgorithm());
                    System.out.println("Cert Public Key Format : "
                            + cert.getPublicKey().getFormat());
                    System.out.println("\n");
                }

            } catch (SSLPeerUnverifiedException e) {
                e.printStackTrace();
            } catch (IOException e){
                e.printStackTrace();
            }
        }
    }
}
