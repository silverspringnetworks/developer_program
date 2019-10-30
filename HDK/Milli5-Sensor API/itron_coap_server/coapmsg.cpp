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

#include "log.h"
#include "coapmsg.h"
#include "coappdu.h"
#include "coapsensoruri.h"
#include "coapobserve.h"
#include "coaputil.h"
#include "arduino_time.h"

/* 
 * intrct_cb_q initialization. FIFO of mid:cb mappings. For now that's all it
 * is, but it may well need to match client and token info (interaction) when
 * multiple clients are supported.
 */

struct intrct_cb_t {
    uint16_t mid;       /* CoAP message ID */
    coap_ack_cb_info_t cbinfo;  /*app cb fn, and param. */
};

#define MID_CB_Q_SZ     4

struct intrct_cb_t intrct_cb_q[MID_CB_Q_SZ];
uint8_t intrct_cb_q_ind;   /* index to add next entry */

/* Max-Age in seconds */
uint32_t coap_max_age_in_seconds = 0;

/*
 * Add entry to the queue at the next (oldest slot). Not thread safe, so
 * assuming synchronization via caller.
 *
 * @param mid: Message ID of CON, and thus ACK.
 * @param cb: callback function to call when ACK arrives.
 * @param cbctx: callback context, param to cb.
 *
 * @return error_t.
 */
error_t
coap_con_add(uint16_t mid, coap_ack_cb_info_t *cbi)
{
    dlog(LOG_DEBUG, "Adding callback for MID: 0x%x\n", mid);
    intrct_cb_q[intrct_cb_q_ind].mid = mid;
    intrct_cb_q[intrct_cb_q_ind].cbinfo = *cbi;
    intrct_cb_q_ind = (intrct_cb_q_ind + 1) % MID_CB_Q_SZ;

    return ERR_OK;
}


/*
 * Find the entry matching mid in the queue, or return ERR_NO_ENTRY.
 * Not thread safe, requirements as above.
 *
 * @param mid: Message ID of ACK.
 *
 * @return return value of cb function if entry found, or ERR_NO_ENTRY.
 */
error_t
coap_ack_rx(uint16_t mid, struct mbuf *m)
{
    uint8_t i;

    dlog(LOG_DEBUG, "Looking up callback for MID: 0x%x\n", mid);
    for (i = 0; i < MID_CB_Q_SZ; i++) {
        if (intrct_cb_q[i].mid == mid) {
            return (intrct_cb_q[i].cbinfo.cb(intrct_cb_q[i].cbinfo.cbctx, m));
        }
    }

    return ERR_NO_ENTRY;
}




/* match encoded option string vs. NUL terminated C string */
int
coap_opt_strcmp(const struct optlv *opt, const char *str)
{
    int diff;

    if (!str) {
        return 1;
    }

    diff = strncmp((char*)opt->ov, str, opt->ol);  /* option is not NUL terminated */
   
    if (diff) {
        return diff;
    } else {
        return (opt->ol - strlen(str));
    }
}

/* construct Uri-Path string from options */ 
/* expects a valid parsed context */
char *
coap_pathstr(const struct coap_msg_ctx *ctx)
{
    struct optlv *opt;
    int ul = 0;
    void *it = NULL;
    /* 
     * This function only called from netmgr thread at present, so naturally
     * synchronous. No need for locking due to this static, yet.
     */
    static char uristr[MAX_URI_LEN];

    uristr[0] = 0;
    /*
     * Iterate through all Uri-Path options.
     */
    while ((opt = copt_get_next_opt_type((const sl_co*)&(ctx->oh), COAP_OPTION_URI_PATH, &it)) != NULL)
	{
        if (ul + opt->ol + 1 > (int)sizeof(uristr))
		{
            /* limit size - error */
            return NULL;
        }

        uristr[ul++] = '/';
        memcpy(uristr + ul, opt->ov, opt->ol);
        uristr[ul + opt->ol] = 0;
        ul += opt->ol;
    }

    return uristr;
}


/* option tlv */
   
int
coap_opt_parse(struct optlv *o, const uint8_t *b, int len)
{
    uint16_t od, ol;
    int i = 1;

    od = b[0] >> 4;
    ol = b[0] & 0xf;

    if (od < 13) {
        i = 1;
    }
    else if (od == 13) {
        od += b[1];
        i = 2;
    }
    else if (od == 14) {
        od = (b[1] << 8) + b[2] + 269;
        i = 3;
    }
    else {
        goto err;
    }

    if (ol < 13) {
        ;   /* nop */
    }
    else if (ol == 13) {
        ol += b[i];
        i++;
    }
    else if (od == 14) {
        ol = (b[i] << 8) + b[i+1] + 269;
    }
    else {
        goto err;
    }
    
    if (len < i || len < i + ol) {
        goto err;
    }

    o->ot = od;
    o->ol = ol;
    o->ov = b + i;
    
    return i + ol;

err:
    return -1;

}


/*
 * Add the specified option to the buffer.
 *
 * @param o: The option tlv to insert (type is delta).
 * @param b: The start of the options.
 * @param len: The length of the buffer.
 *
 * @return: The index of the byte after the option inserted, or -1.
 *      NB This allows the caller to call again with the location to insert
 *      the next option and the option delta.
 */
int
coap_opt_add(const struct optlv *o, uint8_t *b, int len)
{
    uint8_t oh[4];
    uint8_t *op = oh;
    int ohl;

    if ((o->ot > 255 + 13) || (o->ol > 255 + 13)) {
        return -1;
    }
    
    if (o->ot < 13) {
        *op++ = o->ot << 4;
    } else {
        *op++ = 13 << 4;
        *op++ = o->ot - 13;
    }

    if (o->ol < 13) {
        oh[0] |= o->ol;
    } else {
        oh[0] |= 13;
        *op++ = o->ol - 13;
    }

    ohl = op - oh;

    if (ohl + o->ol > len) {
        dlog(LOG_ERR, "Insufficient buffer space to add option\n");
        return 0;
    }

    memcpy(b, oh, ohl);
    memmove(b + ohl, o->ov, o->ol);

    return (o->ol + ohl);
}


static inline uint8_t
co_uint8(const struct optlv *o)
{
    const uint8_t *d = (const uint8_t *) o->ov;
    /* extending to allow parsing 16 bit content format seen on some clients */
    /* rfc7252:  A recipient MUST be prepared to process values with 
    *            leading zero bytes.
    */
    switch (o->ol) {
        case 0:     return 0;
        case 1:     return d[0];
        case 2:     return d[1];
        default:    return 0;
    }
}

uint32_t
co_uint32_n2h(const struct optlv *o)
{
    uint32_t b = 0;

    assert(o->ol <= sizeof(uint32_t));

    switch (o->ol) {
    case 1: b = *(uint8_t *)(o->ov);
            break;
    case 2: b = ntohs(*(uint16_t *)(o->ov));
            break;
    case 3: b = ntohl(*(uint32_t *)(o->ov) << 8);
            break;
    case 4: b = ntohl(*(uint32_t *)(o->ov));
            break;
    }

    return b;
}


uint32_t
co_uint32_h2n(const struct optlv *o)
{
    uint32_t b = 0;

    assert(o->ol <= sizeof(uint32_t));

    switch (o->ol) {
    case 1: b = *(uint8_t *)(o->ov);
            break;
    case 2: b = htons(*(uint16_t *)(o->ov));
            break;
    case 3: b = htonl(*(uint32_t *)(o->ov) << 8);
            break;
    case 4: b = htonl(*(uint32_t *)(o->ov));
            break;
    }

    return b;
}


/*
 * Monotonically increasing value.
 */
uint16_t get_mid_val()
{
    /*
     * Initialise to something other than 0.
     */
    static uint16_t mid_val = 0;

    if (!mid_val) 
	{
        uint32_t epoch = get_rtc_epoch();
        mid_val = (uint16_t) ( epoch & 0x0000FFFF );
    }

	// Increment the message ID
    return mid_val++;
}

static void
coap_msg_log(const struct coap_msg_ctx *ctx)
{
    char *substr;
    struct optlv *op;
    char uriqp[MAX_URI_LEN];

    uriqp[0] = '\0';
    dlog(LOG_DEBUG, "REQ/RSP Type: %s", 
            ctx->type == COAP_T_CONF_VAL ? "CON" : 
            ctx->type == COAP_T_NCONF_VAL ? "NON" :
            ctx->type == COAP_T_ACK_VAL ? "ACK" : "RST");
    if ((ctx->code & COAP_CODE_C_MASK) == COAP_CODE_REQUEST) {
        dlog(LOG_DEBUG, "REQ/ACK Code: %s",
                (ctx->code & COAP_CODE_DD_MASK) == COAP_CODE_GET ? "GET" :
                (ctx->code & COAP_CODE_DD_MASK) == COAP_CODE_POST ? "POST" :
                (ctx->code & COAP_CODE_DD_MASK) == COAP_CODE_PUT ? "PUT" : 
                (ctx->code & COAP_CODE_DD_MASK) == COAP_CODE_DELETE ? 
                "DELETE" : "EMPTY");
    } else {
        dlog(LOG_DEBUG, "RSP Code: %s",
                (ctx->code & COAP_CODE_C_MASK) == COAP_CODE_SUCCESS ? 
                    "Success" :
                (ctx->code & COAP_CODE_C_MASK) == COAP_CODE_CLIENT_ERR ? 
                    "Client Error" : "Server Error");
    }
    substr = coap_pathstr(ctx);
    if (substr[0] != '\0') {
        strcat(uriqp, substr);
    }
    /* Is it possible to get more than one query field? */
    if ((op = copt_get_next_opt_type((const sl_co*)&(ctx->oh), COAP_OPTION_URI_QUERY, NULL))) {
        strcat(uriqp, "?");
        strncat(uriqp, (char *)op->ov, op->ol);
    }
    if (uriqp[0] != '\0') {
        dlog(LOG_INFO, "Uri-Path-Query: %s", uriqp);
    }
}


/*
 * Parse header data into the CoAP message context ctx. Thus the options and
 * payload are NOT parsed.
 *
 * @param ctx: Output, loaded with context info goodness from the packet in
 * data.
 * @param m: The mbuf with the PDU.
 * @return: 0 on success, nonzero error/special handling code. 
 *          special handling required for COAP_RSP_101_SILENT_IGN
 */
static error_t
coap_hdr_parse(struct coap_msg_ctx *ctx, struct mbuf *m)
{
    uint8_t *b = m->m_data;
    error_t rc;

    ctx->msg = m;   /* save mbuf in context - free later */

    /* parse header */
    if ((b[0] & 0xC0) != 0x40) {  /* confirm version 1 */
        rc = ERR_VER_NOT_SUPP;
        goto err;
    }
   
    ctx->hdr = b;
    ctx->type = (b[0] >> 4) & 0x3;
    ctx->tkl  = b[0] & 0xf;
    ctx->code = b[1];
    ctx->mid  = (b[2] << 8) + b[3];
    ctx->plen = 0;  /* Initialise */

    rc = ERR_OK;     /* TBD */

    /* sanity checks 
        token length <= 8
        message length > header + token
    */

    memcpy(ctx->token, b + 4, ctx->tkl);

err:
    return rc;
}


/*
 * Parse data into the CoAP message context ctx.
 * We DON'T modify ctx for some anticipated response, we leave that to the
 * caller. ctx reflects the state of the incoming message.
 *
 * @param ctx: Output, loaded with context info goodness from the packet in
 * data.
 * @param m: The mbuf with the PDU.
 * @param code: CoAP return code.
 * @return: 0 on success, nonzero error/special handling code. 
 */
error_t coap_msg_parse(struct coap_msg_ctx *ctx, struct mbuf *m, uint8_t *code)
{
    int i, osize, mdatalen;
    uint8_t *b = m->m_data; /* assuming single buffer */
    int len = m->m_pktlen;
    struct optlv opt;
    uint16_t ot;
    error_t rc;
    uint32_t obsval;

    /* Default code, indicating everything okay, so far. */
    *code = COAP_RSP_205_CONTENT;

    ddump(LOG_DEBUG, "CoAP REQ decode", b, len);
    
    if ((rc = coap_hdr_parse(ctx, m)) != ERR_OK) {
        goto err;
    }

    i = 4 + ctx->tkl;

    ctx->final = 1; /* default value, not ongoing observe */
    ctx->oidx = i;  /* where options will start, or maybe payload marker */

    if (ctx->code == COAP_EMPTY_MESSAGE) {
        /* ignore everything else */
        return ERR_OK;
    }

    // Make sure the packet length is not greater than what is allocated by m_get()
	mdatalen = get_mbuf_data_size()-16;
    if ( len > mdatalen )
    {
		*code = COAP_RSP_413_REQ_TOO_LARGE;
		return ERR_MSGSIZE;
      
    } // if

    /* collect options */
    /* option tlv - make a struct */

    /*
     * Unrecognized critical options in a non-confirmable message results in a
     * reset.
     *
     * If an option that's unsafe to forward is received, and it's not
     * understood by the proxy, don't forward, but return
     * COAP_RSP_501_NOT_IMPLEMENTED.
     *
     * Silently accept elective options that aren't understood.
     *
     * Descriptions of the reason for the non-2.xx code are to be loaded into
     * payload.
     * ZK - this should be an option (build option?)
     */
    ot = 0;
    while ((osize = coap_opt_parse(&opt, b + i, len - i)) > 0) {
        /* Add, because it's an option delta. */
        ot += opt.ot;
        dlog(LOG_DEBUG, "option type: %u len: %u", ot, opt.ol);
        ddump(LOG_DEBUG, "option", opt.ov, opt.ol);

        /*
         * Add each option to the message context structure to ease
         * manipulation later.
         * Modify opt.ot to be absolute value. Deltas aren't used outside the
         * PDU anyway.
         */
        opt.ot = ot;
        if ((rc = copt_add_opt((sl_co*)&(ctx->oh), &opt)) != ERR_OK) {
            dlog(LOG_ALERT, "Couldn't save option data");
            *code = COAP_RSP_500_INTERNAL_ERROR;
            goto err;
        }
        
        i += osize;

        /*
	     * DSC More to add here, in time.
	     */
        switch (ot) {
        case COAP_OPTION_URI_HOST:          /* Uri-Host */
        case COAP_OPTION_URI_PORT:          /* Uri-Port */
            /* 
             * Handled (nothing to do, for now) and okay to forward.
             */
            break;
        case COAP_OPTION_BLOCK2:            /* Block2    */
                /*FIXME willfully ignoring this option   */
			break;
        case COAP_OPTION_URI_PATH:          /* Uri-Path  */
        case COAP_OPTION_URI_QUERY:         /* Uri-Query */
            break;
        case COAP_OPTION_CONTENT_FORMAT:    /* Content-Format */
            ctx->cf = co_uint8(&opt);
            break;
        case COAP_OPTION_ACCEPT:            /* Accept */
            ctx->cf = co_uint8(&opt)    ;/*FIXME*/
            break;

            /*
             * The options below are the only options that are
             * Unsafe-to-forward that aren't handled further up. Flag these
             * for the proxy code to decide what to do.
             */
        case COAP_OPTION_MAXAGE:
        case COAP_OPTION_PROXY_URI:
        case COAP_OPTION_PROXY_SCHEME:
            break;
        case COAP_OPTION_OBSERVE:
            obsval = co_uint32_n2h(&opt);
            if (obsval == COAP_OBS_REG) {
                ctx->final = 0;
            }
            break;
        default:
            /* unhandled critical option */
            if (COAP_OPTION_CRITICAL(ot)) {
                /* this is an error */
                dlog(LOG_ERR, "unhandled critical option %d\n", ot);
                rc = ERR_OP_NOT_SUPP;
                *code = COAP_RSP_402_BAD_OPTION;
                goto err;
            } else {
               ;     /* ignore all others */
            }
        }
    }

    if (ot && i != len) {
        /* must be separating FF next */
        if (b[i] != 0xFF) {
            dlog(LOG_ERR, "missing option separator FF");
            rc = ERR_BAD_DATA;
            *code = COAP_RSP_415_UNSUPPORTED_CFORMAT;
            goto err;
        }
        i++;
    }

    /* after options - set the payload pointer */
    ctx->hdrlen = i;
    ctx->plen = len - i;
    m->m_pktlen = len;

    coap_msg_log(ctx);
err:
    return rc;
}


/*
 * Parse data into the CoAP response message context ctx.
 * ctx reflects the state of the incoming message.
 *
 * @param ctx: Output, loaded with context info goodness from the packet in
 * data.
 * @param m: The mbuf with the PDU.
 * @return: 0 on success, nonzero error/special handling code. 
 *          special handling required for COAP_RSP_101_SILENT_IGN
 */
error_t
coap_rsp_parse(struct coap_msg_ctx *ctx, struct mbuf *m)
{
    int i, osize;
    uint8_t *b = m->m_data; /* assuming single buffer */
    int len = m->m_pktlen;
    struct optlv opt;
    uint16_t ot;
    error_t rc;

    ddump(LOG_DEBUG, "CoAP RSP decode", b, len);
    
    if ((rc = coap_hdr_parse(ctx, m)) != ERR_OK) {
        goto err;
    }

    i = 4 + ctx->tkl;
    ctx->oidx = i;  /* where options will start, or maybe payload marker */

    /*
     * Simply traverse the options to get the payload info, with the exception
     * of content format and accept for now.
     */
    ot = 0;
    ctx->final = 1;  /* Only observe requires ongoing context. */
    while ((osize = coap_opt_parse(&opt, b + i, len - i)) > 0) {
        /* Add, because it's an option delta. */
        ot += opt.ot;
        dlog(LOG_DEBUG, "option type: %u len: %u", ot, opt.ol);
        ddump(LOG_DEBUG, "option", opt.ov, opt.ol);
        
        /*
         * Add each option to the message context structure to ease
         * manipulation later.
         * Modify opt.ot to be absolute value. Deltas aren't used outside the
         * PDU anyway.
         */
        opt.ot = ot;
        if ((rc = copt_add_opt((sl_co*)&(ctx->oh), &opt)) != ERR_OK) {
            dlog(LOG_ALERT, "Couldn't save option data");
            goto err;
        }
        
        i += osize;

        switch (ot) {
        case COAP_OPTION_OBSERVE:
            ctx->final = 0;
            break;
        case COAP_OPTION_CONTENT_FORMAT:    /* Content-Format */
        case COAP_OPTION_ACCEPT:            /* Accept */
            ctx->cf = co_uint8(&opt)    ;/*FIXME*/
            break;
        default:
            /* ignore all others */
            ;
        }
    }

    if (ot && i != len) {
        /* must be separating FF next */
        if (b[i] != 0xFF) {
            dlog(LOG_ERR, "missing option separator FF");
            rc = ERR_BAD_DATA;
            goto err;
        }
        i++;
    }

    /* after options - set the payload pointer */
    ctx->hdrlen = i;
    ctx->plen = len - i;

    coap_msg_log(ctx);
err:
    return rc;
}

/* initialize response message context based on request + mbuf */
void       
coap_init_rsp(const struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, 
              struct mbuf *m)
{
    struct optlv *op;
    memset(rsp, 0, sizeof(*rsp));
    copt_init((sl_co*)&(rsp->oh));
    /*
     * type will be a bit tricker than this. CON->ACK, except if it's empty,
     * then RST. NON->NON. We need to know more about the message before
     * setting the RSP type.
     */
    rsp->type = req->type;
    rsp->tkl = req->tkl;
    memcpy(rsp->token, req->token, sizeof(rsp->token));
    rsp->mid = req->mid;
    rsp->client = req->client;
    /*
     * Default to the same final setting as req. Parsing the request allows us
     * to know if observe is requested. We may not be able to honour that, but
     * we'll default to doing so.
     */
    rsp->final = req->final;

    /*
     * TODO OBS: Could replicate behaviour by adding the option. Need to know
     * when to add a value. Maybe this isn't required. Check context of calls.
     */
    if ((op = copt_get_next_opt_type((const sl_co*)&(req->oh), COAP_OPTION_OBSERVE, NULL))
               != NULL) {
        uint32_t obsval = co_uint32_n2h(op);
        if (!rsp->final && obsval == COAP_OBS_REG) {
            struct optlv nop;   /* No value ptr, yet */
            /*
             * If this is proxied, then this info isn't used to build the
             * response anyway. The sensor sends the packet. The same applies
             * for local async observe.
             */
            nop.ot = COAP_OPTION_OBSERVE;
            nop.ol = 3;
            if (copt_add_opt((sl_co*)&(rsp->oh), &nop) != ERR_OK) {
                dlog(LOG_ERR, "Couldn't add observe option");
            }
        }
    }

    rsp->msg = m;
}

/*
 * Replace options from ctx into the specified packet at oidx provided they fit
 * before hdrlen. Then pull payload, if present, up to the end of options.
 *
 * @param: ctx: Input, message context structure, populated with data from the
 * existing mbuf, msg.
 *
 * @return: 0 - OK.
 */
error_t
coap_opt_rpl(struct coap_msg_ctx *ctx)
{
    uint16_t ot = 0;    /* option type */
    int sz;
    void *it = NULL;
    struct mbuf *m = ctx->msg;
    uint8_t *b = m->m_data;
    int len;
    int osize;
    int i;
    struct optlv *opt;
    struct optlv dopt;    /* op with type delta */

    i = ctx->oidx;
    dlog(LOG_DEBUG, "oidx: %d, hdrlen: %d, m_pktlen: %d", ctx->oidx, 
            ctx->hdrlen, m->m_pktlen);
    while ((opt = copt_get_next_opt((const sl_co*)&(ctx->oh), &it)) != NULL) {
        /* Deltas go into the PDU. */
        dopt = *opt;
        dopt.ot -= ot;
        ot = opt->ot;
        if ((sz = coap_opt_add(&dopt, &(b[i]), ctx->hdrlen - i)) == 0) {
            dlog(LOG_ERR, "Couldn't add %d option to msg", ot);
            goto err;
        }
        i += sz;
    }

    /* Pull the remaining data up to the end of the options. */
    if (ctx->plen) {
        /* Must be a payload marker in orig and modified */
        b[i++] = 0xff;
    }
    if (i < ctx->hdrlen) {
        memmove(&(b[i]), &(b[ctx->hdrlen]), ctx->plen);
        /* m_copyinto? */
//#ifdef TOOLS_COAP
        m->m_pktlen -= (ctx->hdrlen - i);
//#else
        // FIXME - add this to hbuf m_adj(m, i - ctx->hdrlen);
//#endif
        ctx->hdrlen = i;
        dlog(LOG_DEBUG, "hdrlen: %d, m_pktlen: %d", ctx->hdrlen, 
                m->m_pktlen);

        /* Rebuilt option list so ov is correct. */
        ot = 0;
        i = 4 + ctx->tkl;
        len = m->m_pktlen;
        copt_del_all((sl_co*)&(ctx->oh));
        while ((osize = coap_opt_parse(&dopt, b + i, len - i)) > 0) {
            /* Add, because it's an option delta. */
            ot += dopt.ot;

            /* Make opt->ot absolute. Deltas aren't used outside the PDU. */
            dopt.ot = ot;
            if (copt_add_opt((sl_co*)&(ctx->oh), &dopt) != ERR_OK) {
                dlog(LOG_ALERT, "Couldn't save option data");
                goto err;
            }
            i += osize;
        }
    } else if (i == ctx->hdrlen) {
        dlog(LOG_WARNING, "%s No adjustment necessary when one expected", 
                __FUNCTION__);
    } else {
        dlog(LOG_ERR, "%s replaced options don't fit", __FUNCTION__);
        goto err;
    }

    return ERR_OK;
err:
    return ERR_MSGSIZE;
}


/*
 * Build a response packet out of the context in ctx. Prepends head to existing
 * data.
 *
 * @param ctx: Input, everything we need to know to build the response
 * (including a description?).
 * @param data: Output, the dat buffer containing the packet contents.
 * @param bufsize: Input, the size of the data buffer.
 *
 * @return: Status; 0 - OK.
 */
error_t
coap_msg_response(struct coap_msg_ctx *ctx)
{
    /* 
     * 4 + 8 (max token) + 2 (option and option length, + option) + 1 (option
     * terminator) + 4 observe option. Can be added to later, as required.
     */
    uint8_t b[COAP_OBS_HDR_SZ];  /* as above, +1 */
    error_t rc = ERR_OK;

    int idx = 4;
    struct mbuf *n;


    coap_msg_log(ctx);

    b[0] = COAP_VER | COAP_T_VAL2PDU(ctx->type);
    if (ctx->code == COAP_EMPTY_MESSAGE) {
        b[1] = COAP_EMPTY_MESSAGE;
        b[2] = ctx->mid >> 8;
        b[3] = ctx->mid & 0xFF;
    } else if (COAP_CLASS(ctx->code) == 1){  /* Not expecting this... */
        rc = ERR_INVAL;
        goto done;   /* error - discard */
    } else if (COAP_CLASS(ctx->code) >= 2) {
        b[1] = ctx->code;
        b[2] = ctx->mid >> 8;
        b[3] = ctx->mid & 0xFF;
    } else {
        rc = ERR_INVAL;
        goto done;
    }

    if (ctx->code != COAP_EMPTY_MESSAGE) {
        int onum = 0;
        struct optlv dopt, *op;
        uint32_t opt_val;
        int sz;

        if (ctx->tkl) {
            if (ctx->tkl > 8) {
                rc = ERR_INVAL;
                goto done;   /* invalid - must not be sent */
            }
            memcpy(b + idx, ctx->token, ctx->tkl);
            b[0] |= ctx->tkl;
            idx += ctx->tkl;
        }

        /*
         * Add the observe option if it's present in the context structure.
         * op doesn't contain a value yet, we add that now, so it's really just
         * a flag so we can add the observe value to the packet.
         */
        if ((op = copt_get_next_opt_type((const sl_co*)&(ctx->oh), COAP_OPTION_OBSERVE, NULL))
               != NULL) {
            op->ov = &opt_val;
            dopt = *op;  /* copy original but make type the delta */
            opt_val = get_obs_val();
            opt_val = co_uint32_h2n(&dopt);
            dopt.ot = COAP_OPTION_OBSERVE - onum;
            onum = COAP_OPTION_OBSERVE;
            if ((sz = coap_opt_add(&dopt, &(b[idx]), COAP_OBS_HDR_SZ - idx)) == 0) {
                dlog(LOG_ERR, "Couldn't add Observe option to msg");
                rc = ERR_NO_MEM;
                goto done;
            }
            idx += sz;
        }
			   
        /*
         * Add the Content-Format option (Option 12)
		 *
         */
        if (ctx->plen) 
		{
            dopt.ot = COAP_OPTION_CONTENT_FORMAT - onum;
            onum = COAP_OPTION_CONTENT_FORMAT;
            if (ctx->cf == 0) {     /* text/plain; */
                dopt.ol = 0;
                dopt.ov = &opt_val;
                opt_val = 0;  /* 0 length anyway */
                if ((sz = coap_opt_add(&dopt, &(b[idx]), COAP_OBS_HDR_SZ - idx)) == 0) {
                    dlog(LOG_ERR, "Couldn't add content format option to msg");
                    rc = ERR_NO_MEM;
                    goto done;
                }
                idx += sz;

            } else {
                dopt.ol = 1;
                dopt.ov = &(ctx->cf);
                if ((sz = coap_opt_add(&dopt, &(b[idx]), COAP_OBS_HDR_SZ - idx)) == 0) {
                    dlog(LOG_ERR, "Couldn't add content format option to msg");
                    rc = ERR_NO_MEM;
                    goto done;
                }
                idx += sz;
            }
        
            /*
             * This option (Option 14) is added in observe response.
			 * Thus observe option variable is on the calling stack, 
			 * so it's okay to deref. If we
             * wanted to set this for non-obs responses, we'd have to allocate
             * it in call from coap_s_proc so it's still on stack or heap.
             * (o->ov is the field of concern.
             */
            if ((op = copt_get_next_opt_type((const sl_co*)&(ctx->oh), COAP_OPTION_MAXAGE, 
                            NULL)) != NULL) {
				op->ov = &opt_val;
                dopt = *op;  /* copy original but make type the delta */
				opt_val = coap_max_age_in_seconds;
				opt_val = co_uint32_h2n(&dopt);
                dopt.ot = COAP_OPTION_MAXAGE - onum;
                onum = COAP_OPTION_MAXAGE;
                if ((sz = coap_opt_add(&dopt, &(b[idx]), COAP_OBS_HDR_SZ - idx)) <= 0) {
                    dlog(LOG_ERR, "Couldn't add Max-Age option to msg");
                    rc = ERR_NO_MEM;
                    goto done;
                }
                idx += sz;
            }

        } // plen

		/* End of options */	   
        if (onum && ctx->plen) {
            b[idx++] = 0xFF;    /* end of options */
        }
    }
    assert(idx <= COAP_OBS_HDR_SZ);

    /* prepend header to response */
    n = m_prepend(ctx->msg, idx);
    if (!n) {
        rc = ERR_NO_MEM;
        goto done;
    }
    ctx->msg = n;   /* A new mbuf may be required */
    memcpy(n->m_data, b, idx);

    ddump(LOG_DEBUG, "Response", n->m_data, n->m_pktlen);

done:
    return rc;
}

/**
 * @brief Set Max-Age Option 14
 * 
 *
 */ 
void coap_set_max_age( uint32_t max_age )
{
	coap_max_age_in_seconds = max_age;
	
} // coap_set_max_age

