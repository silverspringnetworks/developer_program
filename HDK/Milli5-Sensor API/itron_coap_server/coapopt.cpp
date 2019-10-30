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


#include <arduino.h>

#include "hbuf.h"
#include "log.h"
#include "coaputil.h"
#include "errors.h"
#include "coappdu.h"
#include "coapextif.h"
#include "coapmsg.h"
#include "coappdu.h"

class sl_co
{
  public:

  coap_opt * slh_first;
};

#define TOOLS_COAP

#ifndef TOOLS_COAP
/******************************************************************************
 * Memory pool code, NIC only.
 *****************************************************************************/
#define CO_MAX  16  /* Maximum number of options concurrently processed */

static OS_MEM *copt_pool;

void
copt_pool_init(void)
{
    void *copt_mem;
    uint8_t err;

    copt_mem = malloc(OS_MEM_SIZE(CO_MAX, sizeof(coap_opt)));
    copt_pool = OSMemCreate(copt_mem, CO_MAX, sizeof(coap_opt), "coap_opts", &err);
    assert(err == 0);
}
#endif  /* TOOLS_COAP */


/*
 * Allocate and initialise a coap_opt. Will be used by the client, and
 * internally to add options
 *
 * @param: None.
 *
 * @return: A pointer to the new object. This is to be passed to all accessor
 * and deallocation functions.
 */
static coap_opt *
copt_alloc(void)
{
    coap_opt *co;

#ifdef TOOLS_COAP
    if ((co = (coap_opt *)malloc(sizeof(coap_opt))) != NULL) {
#else
    if ((co = OSMemGet(copt_pool)) != NULL) {
#endif
        memset(co, 0, sizeof(coap_opt));
    }

    return co;
}


/*
 * Dellocate a coap_opt.
 *
 * @param: co, the object to free.
 *
 * @return: None.
 */
static void
copt_dealloc(coap_opt *co)
{
    /*
     * TODO Return to a pool. Isolate code for Linux/ucos so it looks the
     * same to the caller.
     */
    if (co) {
#ifdef TOOLS_COAP
        free(co);
#else
        OSMemPut(copt_pool, co);
#endif
    }
}


/*
 * Initialse the head.
 */
void
copt_init(struct sl_co *hd)
{
    assert(hd);

    SLIST_INIT(hd);
}


/*
 * Dellocate all attached coap_opt objects and NULLify the head.
 *
 * @param: co, the root object.
 *
 * @return: None.
 */
void
copt_del_all(struct sl_co *hd)
{
    coap_opt *curr, *tmp;

    assert(hd);
    SLIST_FOREACH_SAFE(curr, hd, nxt, tmp) {
        SLIST_REMOVE(hd, curr, coap_opt, nxt);
        copt_dealloc(curr);
    }

    return;
}


/*
 * Add the supplied option to the specified coap_opt head. Assume the supplied
 * head and option isn't NULL. The contents of opt is copied into the newly
 * allocated object, which is inserted into the list.
 *
 * @param: hd, the head anchor
 * @param: opt, the option tlv to add.
 *
 * @return: 0 on sucess.
 */
error_t
copt_add_opt(struct sl_co *hd, struct optlv *opt)
{
    assert(hd);
    coap_opt *nco;       /* New coap option */
    coap_opt *o, *po;    /* option iterator, previous option. */
    error_t rc = ERR_OK;

    assert(hd);
    assert(opt);

    /*
     * Find the node with the correct option value.
     * If present, allocate and append a new object, set object values.
     */
    if ((nco = copt_alloc()) == NULL) {
        rc = ERR_NO_MEM;
        goto done;
    }
    nco->o = *opt;

    if (SLIST_EMPTY(hd) || (SLIST_FIRST(hd)->o.ot > opt->ot)) {
        SLIST_INSERT_HEAD(hd, nco, nxt);
        goto done;
    }
    SLIST_FOREACH(o, hd, nxt) {
        if (SLIST_NEXT(o, nxt) && (SLIST_NEXT(o, nxt)->o.ot > opt->ot)) {
            SLIST_INSERT_AFTER(o, nco, nxt);
            goto done;
        }
        po = o;
    }
    /*
     * Got to the end and didn't find the right location, so it must be
     * appended to the list.
     */
    SLIST_INSERT_AFTER(po, nco, nxt);
    
done:
    return rc;
}


/*
 * Remove the option specified by type and value from the list, and deallocate
 * its storage.
 *
 * @param: hd: The head of the list.
 * @param: opt: A pointer to an option whose content will be sought in the
 * list, and if found, that option deleted from the list.
 *
 * @return: 0 for success.
 */
error_t
copt_del_opt(struct sl_co *hd, struct optlv *opt)
{
    error_t rc = ERR_OK;
    assert(hd);
    coap_opt *o, *tmp; /* option list iterator, and tmp ptr. */
    assert(opt);
    assert(opt->ov);

    SLIST_FOREACH_SAFE(o, hd, nxt, tmp) {
        if ((o->o.ot == opt->ot) && (o->o.ol == opt->ol) && 
            !memcmp(o->o.ov, opt->ov, opt->ol)) {
            /* Found the value */
            SLIST_REMOVE(hd, o, coap_opt, nxt);
            copt_dealloc(o);
            goto done;
        }
    }
    dlog(LOG_DEBUG, "Didn't find option %d to delete.", opt->ot);
    rc = ERR_NO_ENTRY;
done:
    return rc;
}


/*
 * Get the next option in the list of the specified type. If the iterator (*it)
 * is NULL, start at the beginning of that type. If "it" is NULL, just return
 * the first of that type.
 *
 * @param: hd: The head of the list.
 * @param: ot: The option type to match. Nothing else used for matching.
 * @param: it: Iterator used for subsequent calls to get next.
 *
 * @return: The next option (as optlv) in the list, or NULL if no more.
 */
struct optlv *
copt_get_next_opt_type(const struct sl_co *hd, uint16_t ot, void **it)
{
    struct optlv *opt;
    coap_opt **ppco = (coap_opt **)it;
    coap_opt *pco;

    assert(hd);
    pco = SLIST_FIRST(hd);
    if (!ppco) {    /* Allow passing in NULL to find any instances */
        ppco = &pco;
    } else if (!*ppco) {
        *ppco = SLIST_FIRST(hd);
    } else {
        *ppco = SLIST_NEXT(*ppco, nxt);
    }
    while (*ppco) {
        if ((*ppco)->o.ot == ot) {
            break;
        }
        *ppco = SLIST_NEXT(*ppco, nxt);
    }
    if (*ppco) {
        opt = &((*ppco)->o);
    } else {
        opt = NULL;
    }
    return opt;
}


/*
 * Get the next option in the list. Actually returns the option currently
 * indicated by *ppco (it), unless it's NULL in which case it's initialised to
 * co, then it returns the option. Subsequent calls get the next option, until
 * there are no more.
 *
 * @param: hd: The head of the list.
 * @param: it: Iterator used for subsequent calls to get next.
 *
 * @return: The next option (as optlv) in the list, or NULL if no more.
 */
struct optlv *
copt_get_next_opt(const struct sl_co *hd, void **it)
{
    struct optlv *opt;
    coap_opt **ppco = (coap_opt **)it;
    assert(ppco);
    assert(hd);

    if (!*ppco) {
        *ppco = SLIST_FIRST(hd);
    } else {
        *ppco = SLIST_NEXT(*ppco, nxt);
    }
    if (*ppco) {
        opt = &((*ppco)->o);
    } else {
        opt = NULL;
    }
    return opt;
}


/*
 * Remove and deallocate all instances of options of the specified type.
 *
 * @param: hd: The head of the list.
 * @param: ot: The type of option to delete.
 *
 * @return: 0 if anything deleted.
 */
error_t
copt_del_opt_type(struct sl_co *hd, uint16_t ot)
{
    error_t rc = ERR_NO_ENTRY;
    assert(hd);
    coap_opt *o, *tmp;

    SLIST_FOREACH_SAFE(o, hd, nxt, tmp) {
        if (o->o.ot == ot) {
            rc = ERR_OK;
            SLIST_REMOVE(hd, o, coap_opt, nxt);
            copt_dealloc(o);
        }
    }

    if (rc) {
        dlog(LOG_DEBUG, "Didn't find option %d to delete.", ot);
    }

    return rc;
}


/*
 * Dump the contents of the coap_opt object using dlog.
 *
 * @return: None.
 */
void
copt_dump(struct sl_co *hd)
{
    coap_opt *co;

    assert(hd);
    dlog(LOG_DEBUG, "Dumping options:");

    SLIST_FOREACH(co, hd, nxt) {
        dlog(LOG_DEBUG, "option type: %d, len: %d, Val: 0x%x", co->o.ot, 
                co->o.ol, (uint32_t)co->o.ov);
    }
}

enum coap_ups {
    INIT,
    PATH,
    QUERY,
};

static int hexchar(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return -1;
}
/* parse uri string and create sequence of Uri-Path and Uri-Query options */
int
coap_uristr_to_opt(const char *us, uint8_t *buf, int bufsize)
{
    enum coap_ups state = INIT;
    struct optlv o; 
    uint8_t data[80];
    uint8_t *p = data;
    uint8_t *b0 = buf;
    int h,l;
    int rc;
    
    if (!us) {
        return -1;
    }

    o.ov = data;

    while (*us) {
        switch (*us) {
        case '/':   /* path separator */
            if (state == INIT) {
                state = PATH;
                o.ot = COAP_OPTION_URI_PATH; 
            } else if (state == PATH) {
                /* path finished - write option */
                o.ol = p - data;
                rc = coap_opt_add(&o, buf, bufsize - (buf - b0));
                if (rc < 1) {
                    goto err;
                }
                buf += rc;
                state = PATH;   /* starting next <path> */
                o.ot = 0;
                p = data;
            } else {
                /* invalid uri - error */
                goto err;
            }
            break;
        case '%':   /* pct-encoded */
            if (((h = hexchar(us[1])) < 0) ||
                ((l = hexchar(us[2])) < 0)) {
                goto err;
            }
            *p++ = (h << 4) + l;
            us += 2;
            break;
        case '?':   /* first query */
            if (state == PATH){
                /* path finished - write path */
                o.ol = p - data;
                rc = coap_opt_add(&o, buf, bufsize - (buf - b0));
                if (rc < 1) {
                    goto err;
                }
                buf += rc;
                state = QUERY;
                o.ot = COAP_OPTION_URI_QUERY - COAP_OPTION_URI_PATH;
                p = data;                
            } else {
                /* path must be present if a query is given */
                goto err;
            }

            break;
        case '&':   /* additional query */
            if (state == QUERY) {
                /* finished query - write option */
                o.ol = p - data;
                rc = coap_opt_add(&o, buf, bufsize - (buf - b0));
                if (rc < 1) {
                    goto err;
                }
                buf += rc;                
                state = QUERY;  /* starting next <query> */
                o.ot = 0;
                p = data;
            } else {
                goto err;
            }
            break;
        default:
            if (state == INIT) {    /* tolerate missing leading / */
                state = PATH;
                o.ot = COAP_OPTION_URI_PATH;
            }
            
            *p++ = *us;             /* save character to temp buffer */
            if (p - data > (int)sizeof(data)) {
                goto err;
            }
        }
        us++;

    }

    if (p - data > 0) { /* don't create zero length trailing element */
        o.ol = p - data;
        o.ov = data;
        rc = coap_opt_add(&o, buf, bufsize - (buf - b0));
        if (rc < 1) {
            goto err;
        }
        buf += rc;
    }

    return buf - b0;

err:
    return -1;

}
