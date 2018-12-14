/*
Silver Spring Networks, Inc. ("Silver Spring") licenses this software to you and the company (“You” or “Your”) 
a license for this software (“Software”).  
Using the Software constitutes Your acceptance of these terms (“License”).  

License Grant. Subject to Your acceptance and compliance with this License, Silver Spring grants to You, solely for Your own internal business purpose, 
               a non-exclusive, non-transferable license to access and use the Software and the associated user documentation (“Documentation”) 
               for the term and number agreed to be Silver Spring. 

Restrictions. No intellectual property rights are granted under this License and Silver Spring reserves all rights not expressly granted. 
You may not:  
(a) modify or create derivative works of the Software or Documentation; 
(b) assign, transfer, lease or sublicense the Software or Documentation to any third party 
    (other than Your consultants who are bound to written obligations of confidentiality at least as restrictive as those contained in this License); 
and (c) reverse engineer, disassemble, decrypt, extract or otherwise reduce the Software to a human perceivable 
    form or otherwise attempt to determine the source code or algorithms of the Software 
    (unless the foregoing restriction is expressly prohibited by applicable law).
You may not remove or destroy any proprietary, trademark or copyright markings or notices placed on or contained in the Software or Documentation.  
Silver Spring PROVIDES THE SOFTWARE “AS IS” AND MAKES NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY REGARDING OR RELATING TO THE SOFTWARE.  
Silver Spring HEREBY DISCLAIMS ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, 
WHETHER RIGHTS ARISES BY LAW, BY USE AND CUSTOM OR USAGE IN THE TRADE, OR BY COURSE OF DEALING. 
Silver Spring DOES NOT WARRANT THAT THE SOFTWARE OR ANY PORTION THEREOF WILL BE ERROR-FREE.

Termination. This License is effective until terminated. Your rights under this License will terminate automatically without notice if you fail to comply with any of its terms. 
Upon the termination of this License, You must cease all use of the Software and remove it from Your computer.
*/
using System;
using System.IO;
using System.Text;
using System.Xml;               // For XSLT
using System.Xml.Serialization; // For object serialization
using System.Collections;
using System.Runtime.Serialization.Json;


namespace HdkClient
{
    /// <summary>
    /// Object serialization and deserialization support
    /// </summary>
    public class Serialization
    {
        /// <summary>
        /// Serializes an object into XML
        /// </summary>
        /// <param name="pObject">the object to serialize</param>
        /// <returns>an XML equivalent of the object</returns>
        public static String SerializeObject(Object pObject)
        {
            try
            {
                String XmlizedString = null;
                MemoryStream memoryStream = new MemoryStream();
                XmlWriterSettings ws = new XmlWriterSettings();
                ws.OmitXmlDeclaration = true;
                ws.Encoding = Encoding.UTF8;
                ws.Indent = true;
                XmlSerializerNamespaces ns = new XmlSerializerNamespaces();
                ns.Add("", "");
                ns.Add(string.Empty, "");
                XmlWriter xmlWriter = XmlWriter.Create(memoryStream, ws);
                XmlSerializer xs = new XmlSerializer(pObject.GetType(), "");

                xs.Serialize(xmlWriter, pObject, ns);
                XmlizedString = UTF8ByteArrayToString(memoryStream.ToArray());
                return XmlizedString;
            }
            catch (Exception e)
            {
                FileLogger.Write("Error serializing object: " + e.Message + e.StackTrace, FileLogger.LogLevel.FatalError);
                return null;
            }
        }

        /// <summary>
        /// Convert UTF8 byte array to an equivalent string
        /// </summary>
        /// <param name="characters">the byte array</param>
        /// <returns>the string equivalent of the byte array</returns>
        public static string UTF8ByteArrayToString(byte[] characters)
        {
            UTF8Encoding encoding = new UTF8Encoding();
            string constructedString = encoding.GetString(characters);
            return (constructedString);
        }

        /// <summary>
        /// Converts the String to UTF8 Byte array and is used in Deserialization
        /// </summary>
        /// <param name="pXmlString">the string to be converted</param>
        /// <returns>a UTF8 byte array equivalent of the string</returns>
        public static byte[] StringToUTF8ByteArray(string pXmlString)
        {
            UTF8Encoding encoding = new UTF8Encoding();
            byte[] byteArray = encoding.GetBytes(pXmlString);
            return byteArray;
        }

        /// <summary>
        /// Deserialize a string into an equivalent object
        /// </summary>
        /// <param name="pXmlizedString">the string to deserialize</param>
        /// <param name="type">the object type</param>
        /// <returns>an object of type "type"</returns>
        public static Object DeserializeObject(string pXmlizedString, Type type)
        {
            XmlSerializer xs = new XmlSerializer(type);
            MemoryStream memoryStream = new MemoryStream(StringToUTF8ByteArray(pXmlizedString));
            XmlTextWriter xmlTextWriter = new XmlTextWriter(memoryStream, Encoding.UTF8);
            Object x;
            try
            {
                x = xs.Deserialize(memoryStream);
            }
            catch (Exception e)
            {
                //FileLogger.Write("Error deserializing object: " + e.Message + e.StackTrace, FileLogger.LogLevel.FatalError);
                throw e;
            }
            return x;

        }

        /// <summary>
        /// Deserialize a string into an equivalent object
        /// </summary>
        /// <param name="pXmlizedString">the string to deserialize</param>
        /// <param name="type">the object type</param>
        /// <returns>an object of type "type"</returns>
        public static Object DeserializeUtf16Object(string pXmlizedString, Type type)
        {
            string toDeserialze = Serialization.TrimXMLDef(pXmlizedString);
            XmlSerializer xs = new XmlSerializer(type);
            MemoryStream memoryStream = new MemoryStream(StringToByteArray(toDeserialze));
            XmlTextWriter xmlTextWriter = new XmlTextWriter(memoryStream, Encoding.UTF8);
            Object x;
            try
            {
                x = xs.Deserialize(memoryStream);
            }
            catch (Exception e)
            {
                //FileLogger.Write("Error deserializing object: " + e.Message + e.StackTrace, FileLogger.LogLevel.FatalError);
                throw e;
            }
            return x;

        }
        /// <summary>
        /// Converts a string to a byte array
        /// </summary>
        /// <param name="fromString">the string to convert</param>
        /// <returns>a byte array representing the input string</returns>
        public static byte[] StringToByteArray(string fromString)
        {
            // Null in = null out
            if (fromString == null)
                return null;

            // The target byte array is the same length as the input string.
            byte[] arrByte = new byte[fromString.ToCharArray().Length];
            int i = 0;
            // Loop through each charactacter in the input string
            foreach (char ch in fromString.ToCharArray())
            {
                // Convert the current character to a byte equivalent
                arrByte[i] = (byte)ch;
                i++;
            }
            return arrByte;
        }

        /// <summary>
        /// Removes all XML document headers
        /// </summary>
        /// <param name="xml"></param>
        /// <returns>trimmed XML</returns>
        private static string TrimXMLDef(string xml)
        {
            int configPos = xml.IndexOf("?>") + 2;

            if (configPos >= 0)
                xml = xml.Substring(configPos);                 // Remove leading XML stuff

            while (xml.Substring(0, 2) == "\r\n")
                xml = xml.Substring(2);                         // Remove any leading \r\n pair

            return xml;
        }
        /// <summary>
        /// Convert a JSON string to an XML document
        /// </summary>
        /// <param name="json"></param>
        /// <returns></returns>
        public static XmlDocument JsonToXml(string json)
        {
            XmlNode newNode = null;
            XmlNode appendToNode = null;
            XmlDocument returnXmlDoc = new XmlDocument();
            returnXmlDoc.LoadXml("<Document />");
            XmlNode rootNode = returnXmlDoc.SelectSingleNode("Document");
            appendToNode = rootNode;

            string[] arrElementData;
            string[] arrElements = json.Split('\r');
            foreach (string element in arrElements)
            {
                string processElement = element.Replace("\r", "").Replace("\n", "").Replace("\t", "").Trim();
                if ((processElement.IndexOf("}") > -1 || processElement.IndexOf("]") > -1) &&
                    appendToNode != rootNode)
                {
                    appendToNode = appendToNode.ParentNode;
                }
                else if (processElement.IndexOf("[") > -1)
                {
                    processElement = processElement.Replace(":", "").Replace("[", "").Replace("\"", "").Trim();
                    newNode = returnXmlDoc.CreateElement(processElement);
                    appendToNode.AppendChild(newNode);
                    appendToNode = newNode;
                }
                else if (processElement.IndexOf("{") > -1 && processElement.IndexOf(":") > -1)
                {
                    processElement = processElement.Replace(":", "").Replace("{", "").Replace("\"", "").Trim();
                    newNode = returnXmlDoc.CreateElement(processElement);
                    appendToNode.AppendChild(newNode);
                    appendToNode = newNode;
                }
                else
                {
                    if (processElement.IndexOf(":") > -1)
                    {
                        arrElementData = processElement.Replace(": \"", ":").Replace("\",", "").Replace("\"", "").Split(':');
                        newNode = returnXmlDoc.CreateElement(arrElementData[0]);
                        for (int i = 1; i < arrElementData.Length; i++)
                        { newNode.InnerText += arrElementData[i]; }
                        appendToNode.AppendChild(newNode);
                    }
                }
            }

            return returnXmlDoc;
        }
        /// <summary>
        /// Converts an XML document to an equivalent JSON document
        /// </summary>
        /// <param name="xmlDoc">the XML document to convert</param>
        /// <returns>a string representing the JSON document</returns>
        private static string XmlToJSON(XmlDocument xmlDoc)
        {
            StringBuilder sbJSON = new StringBuilder();
            sbJSON.Append("{ ");
            XmlToJSONnode(sbJSON, xmlDoc.DocumentElement, true);
            sbJSON.Append("}");
            return sbJSON.ToString();
        }

        //  XmlToJSONnode:  Output an XmlElement, possibly as part of a higher array
        private static void XmlToJSONnode(StringBuilder sbJSON, XmlElement node, bool showNodeName)
        {
            if (showNodeName)
                sbJSON.Append("\"" + SafeJSON(node.Name) + "\": ");
            sbJSON.Append("{");
            // Build a sorted list of key-value pairs
            //  where   key is case-sensitive nodeName
            //          value is an ArrayList of string or XmlElement
            //  so that we know whether the nodeName is an array or not.
            SortedList childNodeNames = new SortedList();

            //  Add in all node attributes
            if (node.Attributes != null)
                foreach (XmlAttribute attr in node.Attributes)
                    StoreChildNode(childNodeNames, attr.Name, attr.InnerText);

            //  Add in all nodes
            foreach (XmlNode cnode in node.ChildNodes)
            {
                if (cnode is XmlText)
                    StoreChildNode(childNodeNames, "value", cnode.InnerText);
                else if (cnode is XmlElement)
                    StoreChildNode(childNodeNames, cnode.Name, cnode);
            }

            // Now output all stored info
            foreach (string childname in childNodeNames.Keys)
            {
                ArrayList alChild = (ArrayList)childNodeNames[childname];
                if (alChild.Count == 1)
                    OutputNode(childname, alChild[0], sbJSON, true);
                else
                {
                    sbJSON.Append(" \"" + SafeJSON(childname) + "\": [ ");
                    foreach (object Child in alChild)
                        OutputNode(childname, Child, sbJSON, false);
                    sbJSON.Remove(sbJSON.Length - 2, 2);
                    sbJSON.Append(" ], ");
                }
            }
            sbJSON.Remove(sbJSON.Length - 2, 2);
            sbJSON.Append(" }");
        }

        //  StoreChildNode: Store data associated with each nodeName
        //                  so that we know whether the nodeName is an array or not.
        private static void StoreChildNode(SortedList childNodeNames, string nodeName, object nodeValue)
        {
            // Pre-process contraction of XmlElement-s
            if (nodeValue is XmlElement)
            {
                // Convert  <aa></aa> into "aa":null
                //          <aa>xx</aa> into "aa":"xx"
                XmlNode cnode = (XmlNode)nodeValue;
                if (cnode.Attributes.Count == 0)
                {
                    XmlNodeList children = cnode.ChildNodes;
                    if (children.Count == 0)
                        nodeValue = null;
                    else if (children.Count == 1 && (children[0] is XmlText))
                        nodeValue = ((XmlText)(children[0])).InnerText;
                }
            }
            // Add nodeValue to ArrayList associated with each nodeName
            // If nodeName doesn't exist then add it
            object oValuesAL = childNodeNames[nodeName];
            ArrayList ValuesAL;
            if (oValuesAL == null)
            {
                ValuesAL = new ArrayList();
                childNodeNames[nodeName] = ValuesAL;
            }
            else
                ValuesAL = (ArrayList)oValuesAL;
            ValuesAL.Add(nodeValue);
        }

        private static void OutputNode(string childname, object alChild, StringBuilder sbJSON, bool showNodeName)
        {
            if (alChild == null)
            {
                if (showNodeName)
                    sbJSON.Append("\"" + SafeJSON(childname) + "\": ");
                sbJSON.Append("null");
            }
            else if (alChild is string)
            {
                if (showNodeName)
                    sbJSON.Append("\"" + SafeJSON(childname) + "\": ");
                string sChild = (string)alChild;
                sChild = sChild.Trim();
                sbJSON.Append("\"" + SafeJSON(sChild) + "\"");
            }
            else
                XmlToJSONnode(sbJSON, (XmlElement)alChild, showNodeName);
            sbJSON.Append(", ");
        }

        // Make a string safe for JSON
        private static string SafeJSON(string sIn)
        {
            StringBuilder sbOut = new StringBuilder(sIn.Length);
            foreach (char ch in sIn)
            {
                if (Char.IsControl(ch) || ch == '\'')
                {
                    int ich = (int)ch;
                    sbOut.Append(@"\u" + ich.ToString("x4"));
                    continue;
                }
                else if (ch == '\"' || ch == '\\' || ch == '/')
                {
                    sbOut.Append('\\');
                }
                sbOut.Append(ch);
            }
            return sbOut.ToString();
        }

        public static string JSONSerialize(object pObject)

        {

            MemoryStream stream = new MemoryStream();

            DataContractJsonSerializer jsonSer = new DataContractJsonSerializer(pObject.GetType());

            jsonSer.WriteObject(stream, pObject);

            stream.Position = 0;

            StreamReader sr = new StreamReader(stream);

            return sr.ReadToEnd();

        }
        public static object JSONDeserialize(string JSONdata, Type type)

        {

            DataContractJsonSerializer jsonSer = new DataContractJsonSerializer(type);

            MemoryStream stream = new MemoryStream(Encoding.UTF8.GetBytes(JSONdata));

            Object x;

            x = jsonSer.ReadObject(stream);

            return x;

        }

    }
}