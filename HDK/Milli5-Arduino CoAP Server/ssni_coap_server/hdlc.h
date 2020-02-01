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


#ifndef _HDLC_H
#define _HDLC_H

#include <HardwareSerial.h>

#define HDLC_FLAG           (0x7e)

/* The UART time-out period in milliseconds */
#define READ_BUF_TIMEOUT		400

/* The maximum number of bytes we can receive in one chunk over the UART */
#define UART_MAX_BUF_LEN		1024

/* The max payload size in the mNIC */
#define MNIC_MAX_PAYLOAD_SIZE	255

/* Supported header dimensions:
 * Min: format(2) dst(1) src(1) ctrl(1) hcs(2)  - 7 bytes
 * The only option supported in simple HDLC.
 */
#define HDLC_HDR_SIZE       	(7)
#define HDLC_HDR_MAX        	HDLC_HDR_SIZE
#define HDLC_CRC_SIZE			(2)


/* Supported frame types */
#define HDLC_I      (1)
#define HDLC_RR     (2)
#define HDLC_RNR    (3)
#define HDLC_SNRM   (4)
#define HDLC_DISC   (5)
#define HDLC_UA     (6)
#define HDLC_DM     (7)
#define HDLC_FRMR   (8)
#define HDLC_UI     (9)


/* Connection state - idle - active */
/* Data/I Frame transfer states */
#define HDLC_STATE_INIT         (0)
#define HDLC_STATE_IDLE         (1)
#define HDLC_STATE_SEND_I       (2)
#define HDLC_STATE_RECV_DATA    (3)
#define HDLC_STATE_SEND_RR      (5)
#define HDLC_STATE_POLL         (6)
#define HDLC_STATE_SEND_RETRY   (7)
#define HDLC_STATE_RECV_RETRY   (8)
#define HDLC_STATE_ERROR        (9)
#define HDLC_STATE_LAST         (10)

/* Errors */
#define HDLC_ERROR_BASE             (60)
#define HDLC_ERROR_RSP_UNSUPPORTED  (HDLC_ERROR_BASE + 1)
#define HDLC_ERROR_RECV_FRAME       (HDLC_ERROR_BASE + 2)
#define HDLC_ERROR_EXCESS_RXLLC     (HDLC_ERROR_BASE + 3)
#define HDLC_ERROR_NR_MISMATCH      (HDLC_ERROR_BASE + 4)
#define HDLC_ERROR_NS_MISMATCH      (HDLC_ERROR_BASE + 5)
#define HDLC_ERROR_HDR              (HDLC_ERROR_BASE + 6)
#define HDLC_ERROR_SEND_FRAME       (HDLC_ERROR_BASE + 7)
#define HDLC_ERROR_CONNECTION       (HDLC_ERROR_BASE + 8)
#define HDLC_ERROR_BUSY             (HDLC_ERROR_BASE + 9)
#define HDLC_ERROR_TX_RETRY         (HDLC_ERROR_BASE + 10)
#define HDLC_ERROR_RSP_UNEXPECTED   (HDLC_ERROR_BASE + 11)
#define HDLC_ERROR_RX_OVERRUN       (HDLC_ERROR_BASE + 12)
#define HDLC_ERROR_UNKNOWN_STATE    (HDLC_ERROR_BASE + 13)

/* Set pointer to Serial object */
void hdlc_init( HardwareSerial * pUART, uint32_t max_info_len );

/* Frame / data parsers used by send/recv implementers */
struct hdlc_hdr_fields {
    int framelen;
    int hdrlen;
    int infolen;
    int segment;
    uint8_t control;
    uint32_t dst;   /* Encoded address 1,2 or 4 bytes */
    uint32_t src;
};

#define HDLC_PARSE_SHORT    (1)
#define HDLC_PARSE_HDR_ERR  (2)

uint8_t hdlc_addr_encode(uint32_t address);
int hdlc_parse_addr(const uint8_t *buf, int len, uint32_t *eaddr, int *al);
int hdlc_parse_hdr(struct hdlc_hdr_fields *hh, const uint8_t *buf, int buflen);
int hdlc_frm_add_info(const uint8_t *hdr, uint8_t *fhdr, const uint8_t *info, 
                                                int infolen, uint8_t *fmfcs);


struct hdlc_snrm_params {   /* rename - negotiate params */
    uint32_t max_info_tx;
    uint32_t max_info_rx;
    uint32_t window_tx; /* fixed at 1 */
    uint32_t window_rx;
};    


/* Encode/decode control byte */

uint8_t hdlc_control_i(uint8_t nr, uint8_t ns, uint8_t pf);
uint8_t hdlc_control_rr(uint8_t nr, uint8_t pf);
int16_t hdlc_control(uint8_t type, uint8_t pf);

struct hdlc_ctrl {
    uint8_t type;
    uint8_t pf;         /* poll/final */
    uint8_t nr;
    uint8_t ns;
};

int hdlc_parse_control(uint8_t ctrl, struct hdlc_ctrl *hc);

uint8_t hdlc_addr_encode(uint32_t address);


int hdlc_fill_snrm_param(uint8_t *buf, uint32_t buflen, uint32_t *rsplen,
                     struct hdlc_snrm_params *hsp);
int hdlc_parse_snrm_param(const uint8_t *buf, uint32_t buflen, struct hdlc_snrm_params *hsp);

int hdlc_parse_addr(const uint8_t *buf, int len, uint32_t *eaddr, int *al);
int hdlc_hdr(int segment, int16_t fcontrol, uint32_t dst, uint32_t src,
    uint8_t *buf, int *hdrlen);



struct hdlc_conn;
struct dlms_profile;

/* Opens hdlc session connection, based on cfg parameters */
struct mcp;        
int hdlc_session_connect(struct hdlc_conn **hc, int fd,
                         const uint32_t *src, 
                         const uint32_t *dst);

int hdlc_get_connection(struct hdlc_conn **conn, uint32_t dst, uint32_t src, int fd);


/*int hdlc_connect(struct hdlc_conn *conn); internal only now*/
int hdlc_disconnect(struct hdlc_conn *conn);
int hdlc_keep_alive(struct hdlc_conn *conn);

/* server side */
int hdlc_listen_snrm(struct hdlc_conn *conn);

/* Data API */
/* data request - response transfer */
int hdlc_data_txfr(struct hdlc_conn *conn, const void *req, int reqlen,
                                    void *rsp, int *rsplen, int rbuflen);


/*
 * Call is blocked until the server returns with a valid acknowledgement(ack),
 * or an error condition is detected (timeout, bad rx frame etc)  A valid ack
 * could either be an I or an RR frame, with the correct N(R) sequence number.
 * If the acknowledgment is an RR frame, the data will be cached for the next
 * hdlc_data_recv call.
 */
int hdlc_data_send(struct hdlc_conn *conn, const void *req, int reqlen);

/*
 * Call will first return cached read data, if any.  Call is blocked until it
 * receives a frame with the final bit set, or an error condition is detected.
 */
int hdlc_data_recv(struct hdlc_conn *conn, void *rsp, int *rsplen, int rbufsize);

/*
 *
 *
 */
int hdlc_rx(struct hdlc_conn *conn, void *rsp, int *rsplen, int rbufsize);

/* Comm session interface */
struct mss;
void hdlc_comm_init(struct mss **ctx);
int hdlc_uart_connect(struct mss *ctx);
int hdlc_uart_disconnect(struct mss *ctx);
int hdlc_session_established(struct mss *ctx);

const struct hdlc_profile* hdlc_profile_get(void);

#ifdef SSN_x86  /* Unit test API */
int hdlc_test_control(void);
int hdlc_test_frame_encode(void);
int hdlc_test_rsp_ua(void);
#endif

/* Used by standard STAT functions - use only 32 bit values */
struct hdlcstat {
    uint32_t send_snrm;
    uint32_t send_i;
    uint32_t send_rr;
    uint32_t send_other;

    uint32_t recv_i;
    uint32_t recv_ui;
    uint32_t recv_rr;
    uint32_t recv_rnr;
    
    uint32_t seqnum_err;

    uint32_t drop;

    uint32_t send_mbuf_err;
    uint32_t frame_send_err;
    uint32_t recv_frame_timeout;
    uint32_t recv_ss_error;    
    uint32_t recv_mbuf_err;
    uint32_t recv_hdr_err;
    uint32_t recv_large_frame_dropped;
    uint32_t recv_fcs_err;
    uint32_t snrm_retry;
    uint32_t snrm_rsp_retry;
    uint32_t snrm_rsp_err;
    uint32_t data_txfr_abort;
    uint32_t send_bad_hdr;
    uint32_t send_i_err;
    uint32_t ui_frame_overwritten;
    uint32_t recv_recovery;
    uint32_t data_buf_overrun;
    uint32_t send_i_recovery;
};



int hdlc_recv_frame(uint8_t *hdr, uint8_t *info, int framesz, int timeout);
int hdlc_rx(uint8_t *hdr, uint8_t *info, int framesz, int timeout);

int hdlc_send_frame(const uint8_t *hdr, const uint8_t *info, int infolen);

int
hdlc_parse_hdr(struct hdlc_hdr_fields *hh, const uint8_t *buf, int buflen);

void print_hctx_state();

#endif /* _HDLC_H */

