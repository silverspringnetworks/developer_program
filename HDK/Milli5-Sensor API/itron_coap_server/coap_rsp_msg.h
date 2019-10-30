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

#ifndef COAP_RSP_MSG_H
#define COAP_RSP_MSG_H

#include <sys/queue.h>
#include <Arduino.h>
#include "hbuf.h"
#include "errors.h"

/**
 * @brief Make CoAP response message
 * 
 * @param [out] m		A buffer of type struct mbuf
 * @param [out] len		The length of the response
 * @param [in] count	The number of sensor readings
 * @param [in] reading	Pointer to an array of sensor readings or to one reading
 * @param [in] unit		A string containing the unit of measure e.g. "F" for Fahrenheit
 *
 */ 
error_t rsp_msg( struct mbuf * m, uint8_t *len, uint32_t count, float * reading, const char * unit );

#endif /* COAP_RSP_MSG_H */

