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

#ifndef SAPI_ERROR_H_
#define SAPI_ERROR_H_


//////////////////////////////////////////////////////////////////////////
//
// SAPI Error Codes returned by Framework Functions
//
//////////////////////////////////////////////////////////////////////////

typedef enum sapi_error_e
{
    SAPI_ERR_OK					= 0,
	SAPI_ERR_NOT_IMPLEMENTED	= 1,
	SAPI_ERR_NO_ENTRY			= 2,
	SAPI_ERR_NO_MEM				= 3,
	SAPI_ERR_BAD_DATA			= 4,
    SAPI_ERR_FAIL				= 99,
} sapi_error_t;


#endif /* SAPI_ERROR_H_ */
