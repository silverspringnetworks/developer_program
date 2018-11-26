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
    public class DevicesResponseJSON2
    {
        public DeviceSearch deviceSearch { get; set; }
        public class DeviceSearch
        {
            public string docType { get; set; }
            public int totalMatchedCount { get; set; }
            public List<string> docTemplate { get; set; }
            public List<QueryResult> queryResults { get; set; }
        }
        public class QueryResult
        {
            public object nms_id { get; set; }
            public object nms_timestamp { get; set; }
            public object nms_updatedByClient { get; set; }
            public object device_accessPointType { get; set; }
            public string device_administrativeState { get; set; }
            public string device_attribute1 { get; set; }
            public string device_attribute2 { get; set; }
            public object device_attribute3 { get; set; }
            public object device_attribute4 { get; set; }
            public object device_attribute5 { get; set; }
            public object device_billingCycleNumber { get; set; }
            public string device_deviceType { get; set; }
            public object device_iconType { get; set; }
            public bool device_isNetworkBackbone { get; set; }
            public bool device_isSuspect { get; set; }
            public string device_meterType { get; set; }
            public string device_model { get; set; }
            public string device_name { get; set; }
            public string device_operationalState { get; set; }
            public object device_partNumber { get; set; }
            public string device_serialNumber { get; set; }
            public object device_timezoneId { get; set; }
            public string device_utilDeviceId { get; set; }
            public string device_vendor { get; set; }
            public object distributionNetwork_substation_feeder_code { get; set; }
            public object distributionNetwork_substation_id { get; set; }
            public object distributionNetwork_transformer_id { get; set; }
            public object location_address1 { get; set; }
            public object location_address2 { get; set; }
            public object location_antennaHeight { get; set; }
            public object location_city { get; set; }
            public object location_countryCode { get; set; }
            public object location_description { get; set; }
            public object location_groundLevelEstimatedHeight { get; set; }
            public bool location_hasLocation { get; set; }
            public object location_height { get; set; }
            public string location_latLonPoint { get; set; }
            public object location_postalCode { get; set; }
            public object location_regionCode { get; set; }
            public object location_state { get; set; }
            public object location_timezone { get; set; }
            public string location_type { get; set; }
            public string location_utilLocationId { get; set; }
            public string meter_programId { get; set; }
            public object meter_tariffId { get; set; }
            public string nic_architecture { get; set; }
            public string nic_firmwareVersion { get; set; }
            public string nic_hardwareVersion { get; set; }
            public string nic_imageType { get; set; }
            public object nic_isBatteryBacked { get; set; }
            public string nic_macId { get; set; }
            public string nic_serialNumber { get; set; }
            public object nic_state { get; set; }
            public object servicePoint_meter_phase { get; set; }
            public object servicePoint_service_volt_level_code { get; set; }
            public string servicePoint_util_service_pt_id { get; set; }
            public object servicepointTransformer_util_transformer_id { get; set; }
            public object servicepoint_latLonPoint { get; set; }
        }
    }
}

