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
 * Copyright Silver Spring Networks, Inc. 2017-2018
 * All rights reserved.
 *
 * $Id: hbuf.cpp abuvarp $
 *
 * NOTE: This is based on code written by Zoltan Kiss and David Cecil
 *
 */

#include <assert.h>

#include "mshield.h"
#include "hbuf.h"

int malloc_cnt;
int free_cnt;

struct mbuf *
m_get(void)
{
    struct mbuf *m;
    m = (struct mbuf *) malloc(sizeof(*m) + MLEN);
    assert(m);
    m->len = 0;
    m->size = MLEN;
    malloc_cnt++;
    return m;
}

void
m_free(struct mbuf *m)
{
    free(m);
    free_cnt++;
}


struct mbuf *
m_dup(struct mbuf *m)
{

    struct mbuf *n = m_get();

    if (n) {
        memcpy(n, m, sizeof(*m) + MLEN);
        n->len = m->len;
    }

    return n;

}


struct mbuf *
m_prepend(struct mbuf *m, int len)
{

    if (m->len + len > MLEN) {
        return NULL;
    }

    /* make space at the top of the buffer */
    memmove(m->data + len, m->data, m->len);
    m->len += len;

    return m;
}


void *
m_append(struct mbuf *m, int16_t len)
{
    void *d;
    if (m->len + len > MLEN) {
        return NULL;
    }

    d = m->data + m->len;
    m->len += len;
    
    return d;

}


int
m_copydata(struct mbuf *m, uint32_t off, uint32_t len, void *vp)
{
    unsigned int count;
    char *cp = (char *)vp;
    
    if (!m) {
        return -1;
    }
    if ((off < m->len) && (len > 0)) {
        count = min(m->len - off, len);
        memcpy(cp, m->data + off, count);
        len -= count;
    }

    return (len > 0 ? -1 : 0);
}



void
m_adj(struct mbuf *mp, int req_len)
{
    if (mp == NULL) {
        return;
    }
    if ((req_len > mp->len) || (-1*(req_len) > mp->len)) {
        req_len = mp->len;
    }

    if (req_len >= 0) {
        /* Trim from head. */
        mp->len -= req_len;
        memmove(mp->data, &(mp->data[req_len]), mp->len);
    } else {
        /* Trim from tail. */
        mp->len += req_len;
    }
}
