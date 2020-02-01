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

#ifndef _INC_MBUF_H
#define _INC_MBUF_H

#include <arduino.h>

#define MBUF_LITE   (1)

/* limited feature mbuf alternative
 * do not refer to elements directly, always use access macros below
 */
struct mbuf {
    uint16_t len;
    uint16_t size;
    uint8_t data[0];    /* allocated to actual size */
};

typedef struct mbuf * mbuf_ptr_t;

/**
 * @brief Set the size of the mbuf data buffer
 *
 * @param[in] data_buf_size The size of the data buffer of the mbuf
 *
 */
void set_mbuf_data_size( int data_buf_size );


/**
 * @brief Get the size of the mbuf data buffer
 *
 * @param[out] The size of the data buffer of the mbuf
 *
 */
int get_mbuf_data_size();


/**
 * @brief Allocate memory for the mbuf including the data buffer
 *
 *
 */
struct mbuf *m_get();

/**
 * @brief De-allocate the mbuf memory including the data buffer
 *
 * @param[in] m Pointer to the mbuf
 *
 */
void m_free(struct mbuf *m);

/**
 * @brief Append bytes to the mbuf data buffer
 *
 * @param[in] mp Pointer to the mbuf
 * @param[in] len Number of bytes to append
 *
 */
void *m_append(struct mbuf *mp, int16_t len);


/**
 * @brief Prepend bytes to the mbuf data buffer
 *
 * @param[in] m Pointer to the mbuf
 * @param[in] len Number of bytes to prepend
 *
 */
struct mbuf *m_prepend(struct mbuf *m, int len);

/**
 * @brief Duplicate the mbuf
 *
 * @param[in] m Pointer to the mbuf to be duplicated
 *
 */
struct mbuf *m_dup(struct mbuf *);


int m_copydata(struct mbuf *m, uint32_t off, uint32_t len, void *vp);
void m_adj(struct mbuf *mp, int req_len);

/* Compatibility macros for full mbuf - use only these to access mbuf */
#define m_gethdr()  m_get()
#define MGETHDR(m) (m = m_gethdr())
#define M_TRAILINGSPACE(m) (MLEN - (m)->len)
#define m_pktlen    len
#define m_data      data
 /* mtod(m, t)   -- Convert mbuf pointer to data pointer of correct type. */
#define mtod(m, t)      ((t)((m)->m_data))

#endif /* _INC_MBUF_H */
