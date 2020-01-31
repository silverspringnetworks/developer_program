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


/*
 * Copyright SilverSpring Networks 2016.
 * All rights reserved.
 *
 * $Id: errors.h 104606 2016-11-25 03:08:21Z dcecil $
 */

#ifndef _ERRORS_H_
#define _ERRORS_H_

typedef enum error_e {
    ERR_OK          = 0,
    ERR_OP_NOT_SUPP = 1,    /**< Operation not available */
    ERR_AGAIN       = 2,    /**< Resource busy or empty, try again */
    ERR_NO_MEM      = 3,    /**< Cannot allocate memory */
    ERR_INVAL       = 4,    /**< Invalid argument */
    ERR_ADDR_IN_USE = 5,    /**< Address already in use */
    ERR_UNREACH     = 8,    /**< Endpoint is unreachable (no route) */
    ERR_EXISTS      = 9,    /**< Value or file already exists */
    ERR_MSGSIZE     = 10,   /**< Message is too long */
    ERR_IO          = 11,   /**< Input/output error */
    ERR_NO_ENTRY    = 12,   /**< No entry found */
    ERR_NORSP       = 14,
    ERR_TIME_OUT    = 15,   /**< Operation timed out */
    ERR_INPROGRESS  = 16,   /**< Operation still in progress */
    ERR_OVERFLOW    = 19,
    ERR_BAD_DATA    = 20,   /**< Invalid external data (e.g., packet header) */
    ERR_VER_NOT_SUPP= 29,   /**< Unsupported version number */

    ERR_FAIL        = 99,   /**< Unspecified failure */
} error_t;

#endif /* _ERRORS_H_ */
