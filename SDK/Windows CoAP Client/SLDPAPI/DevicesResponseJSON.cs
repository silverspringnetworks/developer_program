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
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SLDPAPI
{
    public class DevicesResponseJSON
    {
        public string docType { get; set; }
        public int totalMatchedCount { get; set; }
        public List<string> docTemplate { get; set; }
        public List<QueryResult> queryResults { get; set; }

        public class QueryResult
        {
            public string bridge_hasRtu { get; set; }
            public string device_accessPointType { get; set; }
            public string device_administrativeState { get; set; }
            public string device_attribute1 { get; set; }
            public string device_attribute2 { get; set; }
            public string device_attribute3 { get; set; }
            public string device_attribute4 { get; set; }
            public string device_attribute5 { get; set; }
            public string device_billingCycleNumber { get; set; }
            public string device_bridgeType { get; set; }
            public string device_catalogNumber { get; set; }
            public string device_d2gType { get; set; }
            public string device_deviceType { get; set; }
            public string device_firmwareImageType { get; set; }
            public string device_firmwareImageType2 { get; set; }
            public string device_firmwareUpdater { get; set; }
            public string device_firmwareUpdater2 { get; set; }
            public string device_firmwareVersion { get; set; }
            public string device_firmwareVersion2 { get; set; }
            public string device_genericType { get; set; }
            public string device_hardwareVersion { get; set; }
            public string device_iconType { get; set; }
            public int? device_id { get; set; }
            public string device_insertedByClientName { get; set; }
            public string device_insertTimestamp { get; set; }
            public string device_isLockedState { get; set; }
            public bool? device_isNetworkBackbone { get; set; }
            public string device_isNodeSim { get; set; }
            public bool? device_isSuspect { get; set; }
            public string device_macId { get; set; }
            public string device_manufactureDate { get; set; }
            public string device_meterType { get; set; }
            public string device_model { get; set; }
            public string device_name { get; set; }
            public string device_naturalKey { get; set; }
            public string device_naturalKeySource { get; set; }
            public string device_operationalState { get; set; }
            public string device_parentId { get; set; }
            public string device_partNumber { get; set; }
            public string device_proxyMacId { get; set; }
            public string device_registrarId { get; set; }
            public string device_rtuDnp3Address { get; set; }
            public string device_rtuFeederName { get; set; }
            public string device_rtuInstallDate { get; set; }
            public string device_rtuJobNumber { get; set; }
            public string device_rtuScadaDbName { get; set; }
            public string device_rtuScadaDnp3Address { get; set; }
            public string device_rtuScadaIpAddress { get; set; }
            public string device_rtuSoftwareVersion { get; set; }
            public string device_rtuSubstationName { get; set; }
            public string device_rtuType { get; set; }
            public string device_security_key { get; set; }
            public string device_serialNumber { get; set; }
            public string device_staticTag { get; set; }
            public int? device_timezoneId { get; set; }
            public string device_tz_id { get; set; }
            public string device_updatedByClientName { get; set; }
            public string device_updateTimestamp { get; set; }
            public string device_utilDeviceId { get; set; }
            public string device_vendor { get; set; }
            public string device_version { get; set; }
            public string distributionNetwork_attribute_1 { get; set; }
            public string distributionNetwork_attribute_2 { get; set; }
            public string distributionNetwork_attribute_3 { get; set; }
            public string distributionNetwork_substation_feeder_code { get; set; }
            public string distributionNetwork_substation_id { get; set; }
            public string distributionNetwork_transformer_id { get; set; }
            public string gs_ApConfigProfileId { get; set; }
            public string gs_deviceAttributes { get; set; }
            public string gs_deviceNetworkId { get; set; }
            public string gs_lqmEnabled { get; set; }
            public string gs_lqmInsync { get; set; }
            public string gs_masterFailoverGroup { get; set; }
            public string gs_networkId { get; set; }
            public string gs_routeTag { get; set; }
            public string gs_ssnDeviceId { get; set; }
            public string gs_teamGroup { get; set; }
            public string han_esi { get; set; }
            public string han_transportType { get; set; }
            public string location_address1 { get; set; }
            public string location_address2 { get; set; }
            public string location_antennaHeight { get; set; }
            public string location_city { get; set; }
            public string location_countryCode { get; set; }
            public string location_description { get; set; }
            public string location_groundLevelEstimatedHeight { get; set; }
            public bool? location_hasLocation { get; set; }
            public string location_height { get; set; }
            public string location_latLonPoint { get; set; }
            public string location_postalCode { get; set; }
            public string location_regionCode { get; set; }
            public string location_state { get; set; }
            public string location_timezone { get; set; }
            public string location_type { get; set; }
            public string location_utilLocationId { get; set; }
            public string meter_firmwareBaseVersion { get; set; }
            public string meter_firmwareUpdater { get; set; }
            public string meter_firmwareVersion { get; set; }
            public int? meter_programId { get; set; }
            public string meter_tariffId { get; set; }
            public string nic_architecture { get; set; }
            public int? nic_blobSequenceNumber { get; set; }
            public string nic_ethernetAddress { get; set; }
            public string nic_ethernetMtu { get; set; }
            public string nic_ethernetSpeed { get; set; }
            public string nic_ethernetStatus { get; set; }
            public string nic_firmwareUpdater { get; set; }
            public string nic_firmwareVersion { get; set; }
            public string nic_hardwareVersion { get; set; }
            public string nic_id { get; set; }
            public string nic_imageType { get; set; }
            public string nic_isBatteryBacked { get; set; }
            public string nic_isBatteryPreferred { get; set; }
            public string nic_isConfSync { get; set; }
            public string nic_isMaster { get; set; }
            public string nic_isMicroAp { get; set; }
            public string nic_layer2SecurityMode { get; set; }
            public string nic_layer7SecurityMode { get; set; }
            public string nic_macId { get; set; }
            public string nic_networkId { get; set; }
            public string nic_opType { get; set; }
            public string nic_proxies { get; set; }
            public string nic_rfIpAddress { get; set; }
            public string nic_securityStats { get; set; }
            public string nic_serialNumber { get; set; }
            public string nic_state { get; set; }
            public string nms_id { get; set; }
            public string nms_timestamp { get; set; }
            public string nms_updatedByClient { get; set; }
            public long? pm_lastSequenceNumber { get; set; }
            public long? pm_nicEpoch { get; set; }
            public long? pm_profileId { get; set; }
            public long? pm_softwareEpoch { get; set; }
            public string registrar_aaaa_records { get; set; }
            public string servicePoint_latLonPoint { get; set; }
            public string servicePoint_meter_phase { get; set; }
            public string servicePoint_service_volt_level_code { get; set; }
            public string servicePoint_util_service_pt_id { get; set; }
            public string servicePointTransformer_util_transformer_id { get; set; }
            public string zigbee_sepVersion { get; set; }
            public string zigbee_state { get; set; }
            public string zigbee_type { get; set; }         
        }
    }
}
