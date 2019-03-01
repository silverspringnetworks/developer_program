package com.itron.enablement.sdk.mqtt.payload;

import co.nstant.in.cbor.CborException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.IntNode;
import com.fasterxml.jackson.databind.node.TextNode;
import org.testng.Assert;
import org.testng.annotations.Test;

import java.io.IOException;

public class Ch4PayloadTransformerTest {

    public static final String EXPECTED_TS = "2018-05-22T16:11:00.000Z";
    public static final String NIC_MAC_ADDRESS = "001350050047dc98";

    @Test
    public void testGasAlert() throws IOException, CborException {
        //{0:"ncmth1",1:"10,lvl"}, {0:"ncmth1",1:"22/05/18/16/11/00,t,10,lvl"}
        String[] payloads = {"a200666e636d746831016631302c6c766c", "A200666E636D74683101781A32322F30352F31382F31362F31312F30302C742C31302C6C766C"};
        boolean oldPayload = true;
        for (String payloadIxHex : payloads) {
            Ch4PayloadTransformer transformer = new Ch4PayloadTransformer();
            String transPayload = transformer.buildPayload(Util.hexStringToByteArray(payloadIxHex), NIC_MAC_ADDRESS);
            JsonNode obs = getObservationNode(transPayload);
            Assert.assertTrue(obs != null, "Missing observation tag");
            Assert.assertTrue(obs.has("gasAlert") && obs.get("gasAlert").equals(new IntNode(10)), "gasAlert not found");
            Assert.assertTrue(oldPayload || (obs.has("timestamp") && obs.get("timestamp").equals(new TextNode(EXPECTED_TS))), "incorrect timestamp");
            Assert.assertTrue(obs.has("macId") && obs.get("macId").equals(new TextNode(NIC_MAC_ADDRESS)), "macId not found");
            oldPayload = !oldPayload;
        }
    }

    @Test
    public void testLowBatteryAlert() throws IOException, CborException {
        //{0:"ncmth1",1:"503,h,NG,s,LB,s"}, {0:"ncmth1",1:"22/05/18/16/11/00,t,503,h,NG,s,LB,s"}
        String payloads[] = {"A200666E636D746831016F3530332C682C4E472C732C4C422C73", "A200666E636D74683101782332322F30352F31382F31362F31312F30302C742C3530332C682C4E472C732C4C422C73"};
        boolean oldPayload = true;
        for (String payloadIxHex : payloads) {
            Ch4PayloadTransformer transformer = new Ch4PayloadTransformer();
            String transPayload = transformer.buildPayload(Util.hexStringToByteArray(payloadIxHex), NIC_MAC_ADDRESS);
            JsonNode obs = getObservationNode(transPayload);
            Assert.assertTrue(obs != null, "Missing observation tag");
            Assert.assertTrue(obs.has("hoursSincePowerUp") && obs.get("hoursSincePowerUp").equals(new IntNode(503)), "hoursSincePowerUp not found");
            Assert.assertTrue(oldPayload || (obs.has("timestamp") && obs.get("timestamp").equals(new TextNode(EXPECTED_TS))), "incorrect timestamp");
            Assert.assertTrue(obs.has("macId") && obs.get("macId").equals(new TextNode(NIC_MAC_ADDRESS)), "macId not found");
            oldPayload = !oldPayload;
        }
    }

    @Test
    public void testSensorFailureAlert() throws IOException, CborException {
        //{0:"ncmth1",1:"NG,s,SF,s"}, {0:"ncmth1",1:"22/05/18/16/11/00,t,count,h,NG,s,SF,s"}
        String payloads[] = {"A200666E636D74683101694E472C732C53462C73", "A200666E636D74683101782532322F30352F31382F31362F31312F30302C742C636F756E742C682C4E472C732C53462C73"};
        boolean oldPayload = true;
        for (String payloadIxHex : payloads) {
            Ch4PayloadTransformer transformer = new Ch4PayloadTransformer();
            String transPayload = transformer.buildPayload(Util.hexStringToByteArray(payloadIxHex), NIC_MAC_ADDRESS);
            JsonNode obs = getObservationNode(transPayload);
            Assert.assertTrue(obs != null, "Missing observation tag");
            Assert.assertTrue(obs.has("deviceAlert") && obs.get("deviceAlert").equals(new TextNode("SF")), "deviceAlert not found");
            Assert.assertTrue(oldPayload || (obs.has("timestamp") && obs.get("timestamp").equals(new TextNode(EXPECTED_TS))), "incorrect timestamp");
            Assert.assertTrue(obs.has("macId") && obs.get("macId").equals(new TextNode(NIC_MAC_ADDRESS)), "macId not found");
            oldPayload = !oldPayload;
        }
    }

    @Test
    public void testNormalHeartbeat() throws IOException, CborException {
        //{0:"ncmth1",1:"65535,h,OK,s"}, {0:"ncmth1",1:"22/05/18/16/11/00,t,65535,h,OK,s"}
        String[] payloads = {"A200666E636D746831016C36353533352C682C4F4B2C73", "A200666E636D74683101782032322F30352F31382F31362F31312F30302C742C36353533352C682C4F4B2C73"};
        boolean oldPayload = true;
        for (String payloadIxHex : payloads) {
            Ch4PayloadTransformer transformer = new Ch4PayloadTransformer();
            String transPayload = transformer.buildPayload(Util.hexStringToByteArray(payloadIxHex), NIC_MAC_ADDRESS);
            JsonNode obs = getObservationNode(transPayload);
            Assert.assertTrue(obs != null, "Missing observation tag");
            Assert.assertTrue(obs.has("hoursSincePowerUp")
                    && obs.get("hoursSincePowerUp").equals(new IntNode(65535)), "hoursSincePowerUp not found");
            Assert.assertTrue(obs.has("batteryHealth")
                    && obs.get("batteryHealth").equals(new TextNode("OK")), "batteryHealth not found");
            Assert.assertTrue(obs.has("deviceHealth")
                    && obs.get("deviceHealth").equals(new TextNode("OK")), "deviceHealth not found");
            Assert.assertTrue(oldPayload || (obs.has("timestamp") && obs.get("timestamp").equals(new TextNode(EXPECTED_TS))), "incorrect timestamp");
            Assert.assertTrue(obs.has("macId") && obs.get("macId").equals(new TextNode(NIC_MAC_ADDRESS)), "macId not found");
            oldPayload = !oldPayload;
        }
    }

    private JsonNode getObservationNode(String payload) throws IOException {
        ObjectMapper mapper = new ObjectMapper();
        JsonNode jsonNode = mapper.readTree(payload);
        ArrayNode observations = (ArrayNode) jsonNode.get("observations");
        if (observations.size() > 0) {
            return observations.get(0);
        }
        return mapper.createObjectNode();
    }
}
