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

#ifndef UTIL_BUF_H
#define UTIL_BUF_H

#include <arduino.h>

uint16_t buf_le16(const void *buf, int idx);
uint32_t buf_le32(const void *buf, int idx);
uint64_t buf_le48(const void *buf, int idx);
uint64_t buf_le64(const void *buf, int idx);
uint16_t buf_be16(const void *buf, int idx);
uint32_t buf_be32(const void *buf, int idx);
uint64_t buf_be48(const void *buf, int idx);
uint64_t buf_be64(const void *buf, int idx);

void buf_wle16(void *buf, int idx, uint16_t val);
void buf_wle32(void *buf, int idx, uint32_t val);
void buf_wbe16(void *buf, int idx, uint16_t val);
void buf_wbe32(void *buf, int idx, uint32_t val);

float buf_befloat(const void *buf, int idx);
float buf_lefloat(const void *buf, int idx);
double buf_bedouble(const void *buf, int idx);
double buf_ledouble(const void *buf, int idx);


#endif
