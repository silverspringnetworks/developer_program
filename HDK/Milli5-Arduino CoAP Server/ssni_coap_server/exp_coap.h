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

#ifndef H_EXP_COAP_
#define H_EXP_COAP_

struct coap_stats {
    uint32_t no_mbufs;      /* Couldn't get mbuf. */
    uint32_t no_mem;       /* Couldn't get mmemory. */
    uint32_t sensors_enabled; /* Successful sensor enable commands. */
    uint32_t sensors_disabled; /* Successful sensor disable commands. */
    uint32_t err_hdlc_send;  /* Error attempting to send to client. */
    uint32_t active_obs;    /* Active observes. */
    uint32_t rx_success;    /* Successful receive of REQ or RSP. */
    uint32_t tx_success;    /* Successful transmit of RSP. */
    uint32_t nretries_exceeded; /* Retry limit exceeded */
};

extern struct coap_stats coap_stats;

#endif
