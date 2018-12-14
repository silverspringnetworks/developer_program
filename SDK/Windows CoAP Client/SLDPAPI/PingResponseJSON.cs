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
    public class PingResponseJSON
    {
        public PingResponse pingResponse { get; set; }
    }
    public class Datum
    {
        public string jobExecutionId { get; set; }
        public string nic_macId { get; set; }
        public object jobScheduleId { get; set; }
        public string command { get; set; }
        public string status { get; set; }
    }

    public class StatusCounts
    {
        public int MARKED_UNREACHABLE { get; set; }
    }

    public class Details
    {
        public int timeSpentWaitingForSyncUnits { get; set; }
        public int devicesSent { get; set; }
        public int timeSpentWaitingForGatewayResponses { get; set; }
        public int devicesFetched { get; set; }
        public string message { get; set; }
        public int totalCount { get; set; }
        public int timeSpentProcessingResponses { get; set; }
        public int timeSpentProcessingSyncUnits { get; set; }
        public int failedCount { get; set; }
        public StatusCounts statusCounts { get; set; }
        public int timeSpentSendingSyncUnits { get; set; }
        public string readerMessage { get; set; }
        public int processedCount { get; set; }
        public int timeSpentMakingGatewayRequests { get; set; }
        public string writerMessage { get; set; }
        public int invalidCount { get; set; }
        public int timeSpentWaitingToMakeGatewayRequests { get; set; }
        public int packetsSent { get; set; }
        public int timeSpentFetchingDevices { get; set; }
    }

    public class Parameters
    {
        public string resultsTarget { get; set; }
        public List<string> deviceList { get; set; }
    }

    public class Job
    {
        public long submitTimestamp { get; set; }
        public Details details { get; set; }
        public string id { get; set; }
        public Parameters parameters { get; set; }
        public long endTimestamp { get; set; }
        public string command { get; set; }
        public long startTimestamp { get; set; }
        public string status { get; set; }
    }

    public class PingResponse
    {
        public List<Datum> data { get; set; }
        public string message { get; set; }
        public Job job { get; set; }
        public string operation { get; set; }
        public int status { get; set; }
    }

    //public class RootObject
    //{
    //    public PingResponse pingResponse { get; set; }
    //}
}
