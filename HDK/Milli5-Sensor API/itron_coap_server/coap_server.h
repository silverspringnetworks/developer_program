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

#ifndef INC_COAP_SERVER_H
#define INC_COAP_SERVER_H

#include "hbuf.h"
#include "coapsensorobs.h"


/**
 * @brief Init HDLCS and the CoAP Server 
 *
 */
void coap_s_init( HardwareSerial * pSerial, uint32_t max_age, uint32_t uart_timeout_ms, uint32_t max_info_len, const char * uri, ObsFuncPtr p );



/**
 * @brief Run HDLCS and the CoAP Server 
 *
 */
void coap_s_run();


/** 
 * @brief Primary CoAP process function.
 * Set up REQ and RSP contexts.
 * Parse the REQ.
 * Deal with special case messages or errors, or hand off for URI servicing.
 * Enable or disable observe, as appropriate.
 * Build a response PDU based on the RSP context, and return the mbuf.
 * @param[out] m pointer to mbuf
 * @return mbuf
 */
mbuf_ptr_t coap_s_proc( mbuf_ptr_t m );



#endif /* INC_COAP_SERVER_H */
