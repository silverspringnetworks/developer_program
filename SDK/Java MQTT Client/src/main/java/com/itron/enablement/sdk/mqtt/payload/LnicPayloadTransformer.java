package com.itron.enablement.sdk.mqtt.payload;

import com.itron.enablement.sdk.mqtt.util.PayloadUtilities;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.json.JSONObject;
import org.json.JSONArray;

import java.util.ArrayList;
import java.util.List;


/**
 * Lnic and Pole Tile IoT device Payload Transformer. LNIC device payload in, Observations JSON out.
 *
 * @author Bob Luben
 */
public class LnicPayloadTransformer
{
    private static final Logger logger = LogManager.getLogger(LnicPayloadTransformer.class.getName());


    // Expected entry point. Treated as a wrapper.
    public List<String> buildPayload(byte[] payload, String macId)
    {
        PayloadUtilities pu = new PayloadUtilities();
        String payloadAsHexStr = pu.bytesToHexString(payload);
        logger.debug("CBOR Raw LNIC Payload Hex: <".concat(payloadAsHexStr).concat(">"));

        // Step 1: process the payload from an LNIC
        LnicPayloadDecoder pd = new LnicPayloadDecoder();
        List<LnicObservations> lolist = pd.processPayload(payload);

        // Step 2: convert decoded payload to the json payload required.
        List<String> jsonLoList = buildOutboundSdpJson(lolist, macId);

        return jsonLoList;
    }


    public List<String> buildOutboundSdpJson(List<LnicObservations> lolist, String macId)
    {
        final String obsRootLabel = "observations";
        final String obsDeviceIdLabel = "deviceId";
        final String obsMacIdLabel = "macId";
        List<String> jsonLoList = new ArrayList<String>();

        for (LnicObservations lo : lolist)
        {
            JSONObject rootJo = new JSONObject();
            JSONArray obsArray = new JSONArray();

            JSONObject aObs = new JSONObject();
            aObs.put(lo.getTimestampLabel(), lo.getObservationTimeStamp());

            Double temperature = lo.getTemperature();
            if (!temperature.isNaN())
            {
                aObs.put(lo.getTemperatureLabel(), lo.getTemperature());
            }
            Double shocksum = lo.getShocksum();
            if (!shocksum.isNaN())
            {
                aObs.put(lo.getShocksumLabel(), lo.getShocksum());
            }
            Double humidity = lo.getHumidity();
            if (!humidity.isNaN())
            {
                aObs.put(lo.getHumidityLabel(), lo.getHumidity());
            }

            if (lo.getTiltLabel() != null)
            {
                JSONObject tilt = new JSONObject();
                for (int indx=0 ; indx < lo.getTiltElementLabels().length ; indx++)
                {
                    tilt.put(lo.getTiltElementLabels()[indx], lo.getTilt()[indx]);
                }
                aObs.put(lo.getTiltLabel(), tilt);
            }

            if (lo.getShockLabel() != null)
            {
                JSONObject shock = new JSONObject();
                for (int indx=0 ; indx < lo.getShockElementLabels().length ; indx++)
                {
                    shock.put(lo.getShockElementLabels()[indx], lo.getShock()[indx]);
                }
                aObs.put(lo.getShockLabel(), shock);
            }

            if (lo.getAlertLabel() != null)
            {
                aObs.put(lo.getAlertLabel(), lo.getAlertName());
            }

            obsArray.put(aObs);

            rootJo.put(obsRootLabel, obsArray);
            rootJo.put(obsDeviceIdLabel, macId);
            rootJo.put(obsMacIdLabel, macId);
            String loAsJson = rootJo.toString();
            logger.debug("Transformed LNIC Observation: {}", loAsJson);
            jsonLoList.add(loAsJson);
        }
        return jsonLoList;
    }
}
