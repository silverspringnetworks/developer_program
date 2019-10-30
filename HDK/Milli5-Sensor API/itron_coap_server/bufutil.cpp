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

#include "bufutil.h"

uint16_t
buf_be16(const void *buf, int idx)
{
    const uint8_t *p = (const uint8_t *) buf;    
    return (p[idx + 1] | p[idx] << 8);
}


void
buf_wbe16(void *buf, int idx, uint16_t val)
{
    uint8_t *p = (uint8_t *) buf;    

    p += idx;
    *p++ = val >> 8;
    *p = val & 0xFF;
	
} // buf_wbe16

void
buf_wle16(void *buf, int idx, uint16_t val)
{
    uint8_t *p = (uint8_t *) buf;    

    p += idx;
    *p++ = val & 0xFF;
    *p = val >> 8;
}

void
buf_wbe32(void *buf, int idx, uint32_t val)
{
    uint8_t *p = (uint8_t *) buf;    

    p += idx;
    *p++ = val >> 24;
    *p++ = (val >> 16) & 0xFF;
    *p++ = (val >> 8) & 0xFF;
    *p = val & 0xFF;
}
