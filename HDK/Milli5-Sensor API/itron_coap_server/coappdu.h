/*

Copyright (c) Silver Spring Networks, Inc. 
All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the ""Software""), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of Silver Spring Networks, Inc. 
shall not be used in advertising or otherwise to promote the sale, use or other 
dealings in this Software without prior written authorization from Silver Spring
Networks, Inc.

*/

#ifndef _COAPPDU_H_
#define _COAPPDU_H_

#define COAP_DEFAULT_PORT       (5683)  /* CoAP default UDP port        */
#define COAP_DEFAULT_MAX_AGE    (60)    /* default max lifetime in seconds */
#define COAP_DEFAULT_VERSION      1     /* only CoAP version supported  */

/* URI definitions */
#define COAP_DEFAULT_SCHEME  "coap"     /* default uri scheme           */
#define COAP_DEFAULT_URI_WELLKNOWN ".well-known/core"



/* CoAP message types */
#define COAP_MESSAGE_CON       0 /* Confirmable     */
#define COAP_MESSAGE_NON       1 /* Non-confirmable */
#define COAP_MESSAGE_ACK       2 /* Acknowledgement */
#define COAP_MESSAGE_RST       3 /* Reset           */

/* 
          Usage of Message Types 
    +----------+-----+-----+-----+-----+
    |          | CON | NON | ACK | RST |
    +----------+-----+-----+-----+-----+
    | Request  | X   | X   | -   | -   |
    | Response | X   | X   | X   | -   |
    | Empty    | *   | -   | X   | X   |
    +----------+-----+-----+-----+-----+
*/


/* CoAP Code encoding 3-bit class / 5-bit detail c.dd */
#define COAP_CODE(n)        (((n)/100 << 5) | (n)%100)
/* Extract code from an encoded CoAP code byte */
#define COAP_CLASS(n) (((n) >> 5) & 0x7)


/* CoAP Method Codes from RFC7252 */
#define COAP_EMPTY_MESSAGE      COAP_CODE(0)

/*
CoAP Method Codes:

    Code    Name                                Reference
    ------- ----------------------------------- ---------------------------
     0.01   GET                                 [IANA]
     0.02   POST                                [IANA]
     0.03   PUT                                 [IANA]
     0.04   DELETE                              [IANA]
     0.07   FETCH                               [I-D.bormann-core-coap-fetch-00]
*/     
#define COAP_REQUEST_GET        COAP_CODE(1)
#define COAP_REQUEST_POST       COAP_CODE(2)
#define COAP_REQUEST_PUT        COAP_CODE(3)
#define COAP_REQUEST_DELETE     COAP_CODE(4)
#define COAP_METHOD_FETCH       COAP_CODE(7)

/*
CoAP Response Codes:

    Code    Name                                Reference
    ------- ----------------------------------- ---------------------------
     2.01   Created                             [IANA]
     2.02   Deleted                             [IANA]
     2.03   Valid                               [IANA]
     2.04   Changed                             [IANA]
     2.05   Content                             [IANA]
     2.31   Continue                            [I-D.ietf-core-block-18]
     4.00   Bad Request                         [IANA]
     4.01   Unauthorized                        [IANA]
     4.02   Bad Option                          [IANA]
     4.03   Forbidden                           [IANA]
     4.04   Not Found                           [IANA]
     4.05   Method Not Allowed                  [IANA]
     4.06   Not Acceptable                      [IANA]
     4.08   Request Entity Incomplete           [I-D.ietf-core-block-18]
     4.12   Precondition Failed                 [IANA]
     4.13   Request Entity Too Large            [IANA]
     4.15   Unsupported Content-Format          [IANA]
     5.00   Internal Server Error               [IANA]
     5.01   Not Implemented                     [IANA]
     5.02   Bad Gateway                         [IANA]
     5.03   Service Unavailable                 [IANA]
     5.04   Gateway Timeout                     [IANA]
     5.05   Proxying Not Supported              [IANA]

*/

#define COAP_RSP_201_CREATED                COAP_CODE(201)
#define COAP_RSP_202_DELETED                COAP_CODE(202)
#define COAP_RSP_203_VALID                  COAP_CODE(203)
#define COAP_RSP_204_CHANGED                COAP_CODE(204)
#define COAP_RSP_205_CONTENT                COAP_CODE(205)
#define COAP_RSP_231_CONTINUE               COAP_CODE(231)
#define COAP_RSP_400_BAD_REQUEST            COAP_CODE(400)
#define COAP_RSP_401_UNAUTHORIZED           COAP_CODE(401)
#define COAP_RSP_402_BAD_OPTION             COAP_CODE(402)
#define COAP_RSP_403_FORBIDDEN              COAP_CODE(403)
#define COAP_RSP_404_NOT_FOUND              COAP_CODE(404)
#define COAP_RSP_405_METHOD_NOT_ALLOWED     COAP_CODE(405)
#define COAP_RSP_406_NOT_ACCEPTABLE         COAP_CODE(406)
#define COAP_RSP_408_REQ_INCOMPLETE         COAP_CODE(408)
#define COAP_RSP_412_PRE_FAILED             COAP_CODE(412)
#define COAP_RSP_413_REQ_TOO_LARGE          COAP_CODE(413)
#define COAP_RSP_415_UNSUPPORTED_CFORMAT    COAP_CODE(415)
#define COAP_RSP_500_INTERNAL_ERROR         COAP_CODE(500)
#define COAP_RSP_501_NOT_IMPLEMENTED        COAP_CODE(501)
#define COAP_RSP_503_SERV_UNAVAILABLE       COAP_CODE(503)
#define COAP_RSP_504_GATEWAY_TIMEOUT        COAP_CODE(504)
#define COAP_RSP_505_PROXY_NOT_SUPPORTED    COAP_CODE(505)

/*
 * To allow us to silently ignore, define our own response class that means no
 * response. 1.xx codes aren't defined for CoAP, so hijacking these for
 * internal error reporting.
 */
#define COAP_RSP_101_SILENT_IGN             COAP_CODE(101)
#define COAP_RSP_102_PROXY                  COAP_CODE(102)

/*
CoAP Option Numbers:

    Number  Name                                Reference
    ------- ----------------------------------- ---------------------------
        0   (Reserved)                          [IANA]
        1   If-Match                            [IANA]
        3   Uri-Host                            [IANA]
        4   ETag                                [IANA]
        5   If-None-Match                       [IANA]
        6   Observe                             [IANA]
        7   Uri-Port                            [IANA]
        8   Location-Path                       [IANA]
       11   Uri-Path                            [IANA]
       12   Content-Format                      [IANA]
       14   Max-Age                             [IANA]
       15   Uri-Query                           [IANA]
       17   Accept                              [IANA]
       20   Location-Query                      [IANA]
       23   Block2                              [I-D.ietf-core-block-18]
       27   Block1                              [I-D.ietf-core-block-18]
       28   Size2                               [I-D.ietf-core-block-18]
       35   Proxy-Uri                           [IANA]
       39   Proxy-Scheme                        [IANA]
       60   Size1                               [IANA]
       64   Authorization                       [I-D.bergmann-ace-dcaf-cose-00]
       65   Authorization-Format                [I-D.bergmann-ace-dcaf-cose-00]
      128   (Reserved)                          [IANA]
      132   (Reserved)                          [IANA]
      136   (Reserved)                          [IANA]
      140   (Reserved)                          [IANA]
      284   No-Response                         [IANA]
            Object-Security                     [I-D.selander-ace-object-security-03]
*/

/*
    CoAP option types - and their sources:

    rfc7252
   +-----+---+---+---+---+----------------+--------+--------+----------+
   | No. | C | U | N | R | Name           | Format | Length | Default  |
   +-----+---+---+---+---+----------------+--------+--------+----------+
   |   1 | x |   |   | x | If-Match       | opaque | 0-8    | (none)   |
   |   3 | x | x | - |   | Uri-Host       | string | 1-255  | (see     |
   |     |   |   |   |   |                |        |        | below)   |
   |   4 |   |   |   | x | ETag           | opaque | 1-8    | (none)   |
   |   5 | x |   |   |   | If-None-Match  | empty  | 0      | (none)   |
   |   7 | x | x | - |   | Uri-Port       | uint   | 0-2    | (see     |
   |     |   |   |   |   |                |        |        | below)   |
   |   8 |   |   |   | x | Location-Path  | string | 0-255  | (none)   |
   |  11 | x | x | - | x | Uri-Path       | string | 0-255  | (none)   |
   |  12 |   |   |   |   | Content-Format | uint   | 0-2    | (none)   |
   |  14 |   | x | - |   | Max-Age        | uint   | 0-4    | 60       |
   |  15 | x | x | - | x | Uri-Query      | string | 0-255  | (none)   |
   |  17 | x |   |   |   | Accept         | uint   | 0-2    | (none)   |
   |  20 |   |   |   | x | Location-Query | string | 0-255  | (none)   |
   |  35 | x | x | - |   | Proxy-Uri      | string | 1-1034 | (none)   |
   |  39 | x | x | - |   | Proxy-Scheme   | string | 1-255  | (none)   |
   |  60 |   |   | x |   | Size1          | uint   | 0-4    | (none)   |
   +-----+---+---+---+---+----------------+--------+--------+----------+

             C=Critical, U=Unsafe, N=NoCacheKey, R=Repeatable


draft-ietf-core-block-18
   +-----+---+---+---+---+--------+--------+--------+---------+
   | No. | C | U | N | R | Name   | Format | Length | Default |
   +-----+---+---+---+---+--------+--------+--------+---------+
   |  23 | C | U | - | - | Block2 | uint   |    0-3 | (none)  |
   |  27 | C | U | - | - | Block1 | uint   |    0-3 | (none)  |
   |  28 |   |   | x |   | Size2  | uint   |    0-4 | (none)  |
   |  60 |   |   | x |   | Size1  | uint   |    0-4 | (none)  |
   +-----+---+---+---+---+--------+--------+--------+---------+

rfc7641
   +-----+---+---+---+---+---------+--------+--------+---------+
   | No. | C | U | N | R | Name    | Format | Length | Default |
   +-----+---+---+---+---+---------+--------+--------+---------+
   |   6 |   | x | - |   | Observe | uint   | 0-3 B  | (none)  |
   +-----+---+---+---+---+---------+--------+--------+---------+

draft-gerdes-ace-dcaf-authorize-04
64, 65
   
draft-tcs-coap-no-response-option-12
   +--------+---+---+---+---+-------------+--------+--------+---------+
   | Number | C | U | N | R |   Name      | Format | Length | Default |
   +--------+---+---+---+---+-------------+--------+--------+---------+
   |   284  |   |   | X |   | No-Response |  uint  |  0-1   |    0    |
   +--------+---+---+---+---+-------------+--------+--------+---------+

*/

#define COAP_OPTION_IF_MATCH        (1) 
#define COAP_OPTION_URI_HOST        (3) 
#define COAP_OPTION_ETAG            (4) 
#define COAP_OPTION_IF_NONE_MATCH   (5)
#define COAP_OPTION_OBSERVE         (6)
#define COAP_OPTION_URI_PORT        (7)
#define COAP_OPTION_LOCATION_PATH   (8)
#define COAP_OPTION_URI_PATH        (11)
#define COAP_OPTION_CONTENT_FORMAT  (12)
#define COAP_OPTION_MAXAGE          (14)
#define COAP_OPTION_URI_QUERY       (15)
#define COAP_OPTION_ACCEPT          (17)
#define COAP_OPTION_LOCATION_QUERY  (20)
#define COAP_OPTION_BLOCK2          (23)
#define COAP_OPTION_BLOCK1          (27)
#define COAP_OPTION_SIZE2           (28)
#define COAP_OPTION_PROXY_URI       (35)
#define COAP_OPTION_PROXY_SCHEME    (39)
#define COAP_OPTION_SIZE1           (60)
#define COAP_OPTION_AUTHORIZATION   (64)
#define COAP_OPTION_AUTH_FORMAT     (65)
#define COAP_OPTION_NO_RESPONSE     (284)
/* do we need a 'max' here */

/* 
    rfc7252
   [..] derive the characteristics of an option number "onum"
   Critical = (onum & 1);
   UnSafe = (onum & 2);
   NoCacheKey = ((onum & 0x1e) == 0x1c);
*/
#define COAP_OPTION_CRITICAL(onum)          ((onum) & 1)
#define COAP_OPTION_UNSAFE(onum)            ((onum) & 2)
#define COAP_OPTION_NOCACHEKEY(onum)        (((onum) & 0x1e) == 0x1c)


/* CoAP Content-Format Numbers:

    Number  Name                                Reference
    ------- ----------------------------------- ---------------------------
        0   text/plain;charset=utf-8            [IANA]
        2   application/csv                     [IANA]
       40   application/link-format             [IANA]
       41   application/xml                     [IANA]
       42   application/octet-stream            [IANA]
       47   application/exi                     [IANA]
       50   application/json                    [IANA]
       60   application/cbor                    [IANA]
            application/dcaf+cbor               [I-D.gerdes-ace-dcaf-authorize-04]
*/            
#define COAP_CF_TEXT_PLAIN                  (0)
#define COAP_CF_CSV                 		(2)
#define COAP_CF_APPLICATION_LINK_FORMAT     (40)
#define COAP_CF_APPLICATION_XML             (41)
#define COAP_CF_APPLICATION_OCTET_STREAM    (42)
#define COAP_CF_APPLICATION_EXI             (47)
#define COAP_CF_APPLICATION_JSON            (50)
#define COAP_CF_APPLICATION_CBOR            (60) 

/* CF_TEXT_PLAIN (0) is _not_ the default media type.
   Server needs to differentiate whether specific type was requested
 */

/* prefixing non-zero length payload, following header, tokan and options */
#define COAP_PAYLOAD_MARKER             (0xFF)


#endif /* _COAPPDU_H_ */


