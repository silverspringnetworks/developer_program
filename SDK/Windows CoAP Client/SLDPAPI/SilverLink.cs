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
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

namespace SLDPAPI
{
    public class Credentials
    {
        public string clientId { get; set; }
        public string clientSecret { get; set; }
    }
    public class SilverLink
    {
        /// <summary>
        /// Ex: https://api.silverlink.silverspringnet.com
        /// </summary>
        public string BaseURL { get; set; }
        /// <summary>
        /// Ex: fdaklj@@@##sdfj234241aslkfdjaslkj23232
        /// </summary>
        public string ClientID { get; set; }
        /// <summary>
        /// Ex: afdsajklsdafjlksdjfaljdslkaajslkfjaklsjdfkaljdskfljadfsklljfkds
        /// </summary>
        public string Secret { get; set; }
        public bool DontRequireAuth { get; set; }
        /// <summary>
        /// Ex: cus, detdemo, comed,
        /// </summary>
        public string Solution { get; set; }
        public string TokenPath { get; set; }

        public string SilverLinkToken { get; set; }

        const string ProjectDoesntExistMsg = "Project doesn't exist in the SilverLink Platform.";



        public string Post(string path, string body)
        {
            if (string.IsNullOrWhiteSpace(SilverLinkToken))
                GetSilverLinkToken();

            using (var client = new HttpClient())
            {
                client.DefaultRequestHeaders.Clear();
                client.DefaultRequestHeaders.Add("Content-type", "application/json");
                client.DefaultRequestHeaders.Add("Authorization", SilverLinkToken);///???

                var content = new StringContent(body);
                //using (var response = client.GetAsync("https://poc.api.dev.ssniot.cloud/api" + path).Result)
                //https://poc.api.dev.ssniot.cloud/api/solutions/lpoc/devices

                using (var response = client.PostAsync(BaseURL + path, content).Result) 
                {
                    if (response.IsSuccessStatusCode)
                    {
                        return ReadResponseJSON(response);
                    }
                }
            }
            return "";
        }



        public string Get(string path)
        {
            if (string.IsNullOrWhiteSpace(SilverLinkToken))
                GetSilverLinkToken();

            using (var client = new HttpClient())
            {
                client.DefaultRequestHeaders.Clear();
                //client.DefaultRequestHeaders.Add("Accept", "application/json");
                client.DefaultRequestHeaders.Add("Authorization", SilverLinkToken);

                using (var response = client.GetAsync(BaseURL + path).Result)
                {
                    if (response.IsSuccessStatusCode)
                    {
                        return ReadResponseJSON(response);
                    }
                }
            }
            return "";
        }
        private static string ReadResponseJSON(HttpResponseMessage response)
        {
            using (var content = response.Content.ReadAsStreamAsync().Result)
            {
                using (var reader = new StreamReader(content))
                {
                    var json = reader.ReadToEnd();
                    return json;
                }
            }
        }

        private static string DateToSLDPFormat(DateTime date)
        {
            return date.ToString("yyyy/MM/dd-hh:mm:ss");
        }

        private void GetSilverLinkToken()
        {
            using (var http = new HttpClient())
            {
                var tokenURL = BaseURL + TokenPath;
                Credentials c = new Credentials();
                c.clientId = ClientID;
                c.clientSecret = Secret;
                string jsonBody = JsonConvert.SerializeObject(c);
                var con = new StringContent(jsonBody);

                con.Headers.Clear();
                http.DefaultRequestHeaders.Clear();
                http.DefaultRequestHeaders.Add("Accept", "application/json");

                //using (var response = http.PostAsync(new Uri(tokenURL), con).Result)
                HttpResponseMessage response;
                try
                {
                    using (response = http.PostAsync(new Uri(tokenURL), con).Result)
                    {
                        Stream content;
                        using (content = response.Content.ReadAsStreamAsync().Result)
                        {
                            StreamReader reader;
                            using (reader = new StreamReader(content))
                            {
                                var rawResult = reader.ReadToEnd();
                                var json = JObject.Parse(rawResult);
                                foreach (var a in json.Properties().Where(a => a.Name == "accessToken"))
                                {
                                    SilverLinkToken = a.Value.ToString();
                                }
                            }
                        }
                    }
                }
                catch(Exception badpost)
                {
                    throw badpost;
                }
            }
        }
    }
}
