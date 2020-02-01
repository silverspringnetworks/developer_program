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

#ifndef _CRC_XMODEM_H_
#define _CRC_XMODEM_H_

#include <arduino.h>

uint16_t crc_xmodem_init(void);
uint16_t crc_xmodem(uint16_t crc, const void *addr, unsigned int len);


/*! @brief
 * This crc16 implementation matches PPP FCS calculation, also used for HDLC
 * The resulting 16 bit value needs to be inverted and appended LSB first 
 * after the data buffer.
 * CRC of the resulting buffer will yield F0B8 in absence of errors.
 */
#define CRC16_FINAL     (0xf0b8)  /* Good final FCS value */
uint16_t crc16_init(void);
uint16_t crc16(uint16_t crc, const  void *addr_v, unsigned int len);
int crc16_validate(const void *addr_v, unsigned int len);

/* CRC-DNP implementation for dnp3/m-bus */
uint16_t crc_dnp(const uint8_t *data, int len);

#endif
