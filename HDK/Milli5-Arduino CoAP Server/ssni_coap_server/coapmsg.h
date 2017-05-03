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

#ifndef INC_COAPMSG_H
#define INC_COAPMSG_H

#include <sys/queue.h>
#include <arduino.h>
#include "coapextif.h"

#define COAP_VER_VAL        (1)
#define COAP_VER            (COAP_VER_VAL << 6)
#define COAP_T_VAL2PDU(val) ((val) << 4)
#define COAP_T_CONF_VAL     (0)
#define COAP_T_CONF         (COAP_T_VAL2PDU(COAP_T_CONF_VAL))
#define COAP_T_NCONF_VAL    (1)
#define COAP_T_NCONF        (COAP_T_VAL2PDU(COAP_T_NCONF_VAL))
#define COAP_T_ACK_VAL      (2)
#define COAP_T_ACK          (COAP_T_VAL2PDU(COAP_T_ACK_VAL))
#define COAP_T_RESET_VAL    (3)
#define COAP_T_RESET        (COAP_T_VAL2PDU(COAP_T_RESET_VAL))
#define COAP_TKL(n)         ((n) & 0x0F)

#define COAP_CODE_REQUEST_VAL   (0)
#define COAP_CODE_REQUEST       (COAP_CODE_REQUEST_VAL << 5)
#define COAP_CODE_SUCCESS_VAL   (2)
#define COAP_CODE_SUCCESS       (COAP_CODE_SUCCESS_VAL << 5)
#define COAP_CODE_CLIENT_ERR_VAL (4)
#define COAP_CODE_CLIENT_ERR    (COAP_CODE_CLIENT_ERR_VAL << 5)
#define COAP_CODE_SERVER_ERR_VAL (5)
#define COAP_CODE_SERVER_ERR    (COAP_CODE_SERVER_ERR_VAL << 5)
#define COAP_CODE_C_MASK        0xE0
#define COAP_CODE_DD_MASK       (~COAP_CODE_C_MASK & 0xFF)

#define COAP_CODE_EMPTY         (0)

#define COAP_CODE_GET           (1)     /* 0.01 */
#define COAP_CODE_POST          (2)     /* 0.02 */
#define COAP_CODE_PUT           (3)     /* 0.03 */
#define COAP_CODE_DELETE        (4)     /* 0.04 */

#define SID_MAX_LEN             32      /* Sensor component of URI, max */

/* maximum length of a Uri string */
#define OPT_STR_MAX     		(99)

/* 
 * 4 + 8 (max token) + 2 (option and option length, + option) + 1 (option
 * terminator) + 4 observe option. Can be added to later, as required.
 */
#define COAP_OBS_HDR_SZ     	(28)

struct coap_msg_ctx {
    /* version always 01 on send, silently discarded on error recv */
    uint8_t type;               /* conf(0), nconf(1), ack(2), reset(3) */
    uint8_t tkl;                /* token length 0-8 bytes*/
    uint8_t  code;              /* Code - Request Method / Response Code */
    uint16_t mid;               /* message id */
    uint8_t token[8];           /* Token */
   
    void    *hdr;               /* header always fits into the first mbuf */
    int     hdrlen;
    /* options - todo. DSC: UTF8? */
    
    int         oidx;           /* index of the first option */
    char        sid[SID_MAX_LEN];/* Sensor component of URI, if present */
    uint8_t     cf;             /* content-format */
    int         plen;           /* payload length (starting at [hdrlen] */

    void        *client;        /* Opaque client handle */
    int         final;          /* One shot REQ/RSP or ongoing aka observe */
    SLIST_HEAD(sl_co, coap_opt) oh;  /* Options list head. */

    struct mbuf *msg;           /* complete message - header + payload */

};

struct mbuf;

struct optlv {
    uint16_t ot;				/* Option type				*/
    uint16_t ol;				/* Option length?			*/ 
    const void *ov;				/* Pointer to option value	*/
};

typedef struct coap_opt {
    struct optlv          o;    /* option TLV, absolute values. */
    SLIST_ENTRY(coap_opt) nxt;  /* next option */
} coap_opt;

int coap_opt_strncmp(const struct optlv *opt, const char *str, uint8_t len);
int coap_opt_strcmp(const struct optlv *opt, const char *str);
char *coap_pathstr(const struct coap_msg_ctx *ctx);

int coap_opt_parse(struct optlv *o, const uint8_t *b, int len);

error_t coap_msg_parse(struct coap_msg_ctx *ctx, struct mbuf *m, uint8_t *code);
error_t coap_rsp_parse(struct coap_msg_ctx *ctx, struct mbuf *m);
error_t coap_msg_response(struct coap_msg_ctx *rsp);

void coap_init_rsp(const struct coap_msg_ctx *req, struct coap_msg_ctx *rsp, 
                    struct mbuf *m);

uint16_t get_mid_val(void);
uint32_t co_uint32_h2n(const struct optlv *o);
uint32_t co_uint32_n2h(const struct optlv *o);

int coap_opt_add(const struct optlv *o, uint8_t *b, int len);
error_t coap_opt_rpl(struct coap_msg_ctx *ctx);

/* coap_opt accessor functions */
void copt_init(struct sl_co *hd);
error_t copt_add_opt(struct sl_co *hd, struct optlv *opt);
struct optlv *copt_get_next_opt_type(const struct sl_co *hd, uint16_t ot, 
        void **it);
error_t copt_del_opt(struct sl_co *hd, struct optlv *opt);
error_t copt_del_opt_type(struct sl_co *hd, uint16_t ot);
void copt_del_all(struct sl_co *hd);
struct optlv *copt_get_next_opt(const struct sl_co *co, void **it);
void copt_dump(struct sl_co *hd);
int coap_uristr_to_opt(const char *us, uint8_t *buf, int bufsize); 

/*** CON/ACK support. ***/
/* Add entry to midcb registry. Called when sending CON. */
error_t coap_con_add(uint16_t mid, coap_ack_cb_info_t *cbi);
/* Notify callback when ACK rxed. */
error_t coap_ack_rx(uint16_t mid, struct mbuf *m);

/**
 * @brief Set Max-Age Option 14
 * 
 *
 */ 
void coap_set_max_age( uint32_t max_age );


/******************************************************************************
 * Memory pool code, NIC only.
 *****************************************************************************/
void copt_pool_init(void);


#endif /* INC_COAPMGS_H */

