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

#include <assert.h>
#include <arduino.h>

#include "hbuf.h"
#include "hdlcs.h"
#include "hdlc.h"
#include "bufutil.h"
#include "crc_xmodem.h"
#include "log.h"


extern int verbose;


struct hdlcs_cfg {
    uint32_t max_info_tx;
    uint32_t max_info_rx;
};


enum hdlcss {
    HSS_INIT,
    HSS_DISC,
    HSS_NORM
};

#define INCM8(i)    ((i + 1) & 0x07)

struct hdlcs_state
{
    int     open;
    
    struct hdlcs_cfg    cfg;
    
    enum    hdlcss state;

    uint8_t esrc;
    uint8_t edst;

    uint8_t vs;
    uint8_t vr;
    uint8_t vs_ack;
    uint8_t vr_ack;

    hdlcs_data_handler icb; /* not supported */
    struct mbuf *recv;  /* accumulating incoming data */
    int r_complete;
};

// Declare hss
struct hdlcs_state hss;


/* secondary station handlers for known frame types */
static int hdlcs_snrm(void);
static int hdlcs_disc(void);
static int hdlcs_i(struct mbuf *d);
/* error response frames */
static int hdlcs_dm(void);
static int hdlcs_frmr(void);


// Allocate an mbuf
static void hdlcs_get_buf(int size)
{
	// Set the size of the mbuf data buffer
	set_mbuf_data_size(size);
	
	// Allocate mbuf
    hss.recv = m_get();
    assert(hss.recv);
}


// Time-out period in ms of the UART
static uint32_t uart_timeout_ms		= 0;

/* Open HDLCS connection */
error_t hdlcs_open( HardwareSerial * pUART, uint32_t timeout_ms, uint32_t max_info_len )
{
	// Check that we are not already open
    if (hss.open) 
	{
        /* already open - err */
        return ERR_FAIL;
    }
	
	// Check that the specified max payload size is not larger than the corresponding size on the mNIC
	if ( max_info_len > MNIC_MAX_PAYLOAD_SIZE )
	{
		dlog( LOG_DEBUG, "The max payload size specified is too large: %d bytes. The maximum allowed is %d bytes ", max_info_len, MNIC_MAX_PAYLOAD_SIZE );
		return ERR_FAIL;
		
	} // if
	
	// Init HDLC UART
	hdlc_init( pUART, max_info_len );

	// Set the time-out period of the UART
	uart_timeout_ms = timeout_ms;
    
    /* set up base state */
    memset(&hss, 0, sizeof(hss));

	/* Set flag saying that we are open */
    hss.open = 1;

    /* initialize config to defaults */
    hss.cfg.max_info_tx = max_info_len;
    hss.cfg.max_info_rx = max_info_len;

    /* start in disconnected state */
    hss.state = HSS_DISC;

	/* Allocate the mbuf */
    hdlcs_get_buf(max_info_len);
   
    /* my address */
    hss.esrc = hdlc_addr_encode(1);
    /* update this when the connection is made.  fixed now */
    hss.edst = hdlc_addr_encode(1);

    return ERR_OK;
}


int
hdlcs_close(void)
{
    if (hss.recv) {
        m_free(hss.recv);
    }

    memset(&hss, 0, sizeof(hss));


    return 0;
}


// This pointer will be assigned when an Observe message is ready to be sent
extern struct mbuf *pending_rsp;

/* The main HDLC Secondary station state machine */
int hdlcs_run(void)
{
    uint8_t hdr[HDLC_HDR_SIZE];
    struct hdlc_hdr_fields hh;
    struct hdlc_ctrl hc;
    int rc;

    /* Check for HDLC frame */
    rc = hdlc_rx( hdr, hss.recv->data, hss.recv->size, uart_timeout_ms );  
	if ( rc <= 0 )
	{
        return 0;
    }

    /* Parse header */
	rc = hdlc_parse_hdr( &hh, hdr, HDLC_HDR_SIZE );
	if (rc) 
	{
        /* error parsing packet header -- drop */
        return 0;
    }

	/* Parse control */
	rc = hdlc_parse_control( hh.control, &hc );
    if (rc) 
	{
        /* error decoding control info -- drop - or - frame reject response */
        return 0;
    }

    hss.recv->len = hh.infolen;
    
    dlog(LOG_DEBUG, "Process incoming ctrl %02x in state %d", hh.control, hss.state);

    /* Free last packet if it's acked */
    if (hc.nr == INCM8(hss.vs)) 
	{
        dlog(LOG_DEBUG, "response rxed at primary");
        hss.vs = INCM8(hss.vs);
    }

    switch (hss.state) 
	{
    case HSS_DISC:
        /* reject all frames except SNRM */
        /* send DM response */
        if (hc.type == HDLC_SNRM) {
            rc = hdlcs_snrm();
        }
        else {
            /* reject all with DM response */
            rc = hdlcs_dm();
        }
        break;
    case HSS_NORM:
        /* normal mode processing */
        if (hc.type == HDLC_SNRM) {
            dlog( LOG_DEBUG, "HDLC_SNRM" );
            rc = hdlcs_snrm();
        }
        else if (hc.type == HDLC_I) {
            dlog( LOG_DEBUG, "HDLC_I" );
            /* update seqnums */
            if (hc.ns != hss.vr) {
                dlog(LOG_ERR, "Unexpected seqnum N(S) = %d  V(R) = %d", 
                            hc.ns, hss.vr);
            }
            else {
                hss.vr = INCM8(hss.vr);
            }
            rc = hdlcs_i(hss.recv);
        }
        else if (hc.type == HDLC_RR) {
            dlog( LOG_DEBUG, "HDLC_RR" );
            /* process seqnum - retransmit if necessary */
            dlog(LOG_DEBUG, "hc.nr: %d, hss.vs: %d", hc.nr, hss.vs);
            rc = hdlcs_rr();
        }

        else if (hc.type == HDLC_DISC) {
            dlog( LOG_DEBUG, "HDLC_DISC" );
            m_free(pending_rsp);
            pending_rsp = NULL;
            dlog(LOG_DEBUG, "%s:%d Cleared pending_rsp", __FUNCTION__, __LINE__);
            rc = hdlcs_disc();
        }
        else {
            /* discard unhandled type */
            rc = hdlcs_frmr();
        }
        break;
    default:
        /* unknown state - error */
		dlog( LOG_DEBUG, "Error - unknown state: %d", hss.state );
        return 1;
    }

	// Log
    dlog( LOG_DEBUG, "hdlcs_run() - %d", rc );
    return 0;

} // hdlcs_run()


/*
 * @brief Check if we are in connected state
 *
 */
boolean hdlcs_is_connected()
{
	boolean is_connected = false;
	if ( hss.state == HSS_NORM )
	{
		is_connected = true;

	} // if
	return is_connected;

} // hdlcs_is_connected


struct mbuf * hdlcs_read(void)
{
    struct mbuf *r;
    
    if (hss.r_complete) {
        /* using a duplicate here */
        r = m_dup(hss.recv);
        assert(r);
        hss.recv->len = 0;
        hss.r_complete = 0;
        memset(hss.recv->data, 0, hss.recv->size);

		dlog( LOG_DEBUG, "hdlcs_read() - %x", r );
        return r;
    }
    
	return NULL;

} // hdlcs_read()

int
hdlcs_write(const void *data, uint16_t len)
{
    uint8_t hdr[HDLC_HDR_SIZE];
    int hdrlen;
    int rc;

    /* no segmentation support at this time  - write sends I frm directly */
    (void)hdlc_hdr(0, hdlc_control_i(hss.vr, hss.vs, 1),
                          hss.esrc, hss.edst, hdr, &hdrlen);
    
    rc = hdlc_send_frame(hdr, (const uint8_t *)data, len);
    
    return rc;
}


static int hdlcs_snrm(void)
{
    uint8_t hdr[HDLC_HDR_SIZE];
    uint8_t param_info[26];
    uint32_t rsplen;
    int rc;

    struct hdlc_snrm_params hsp;

    int hdrlen;


    hss.state = HSS_NORM;
            
     /* reinit state */
    dlog(LOG_DEBUG, "enter normal mode");
            
    /* respond with UA */
    hdlc_hdr(0, hdlc_control(HDLC_UA, 1), hss.esrc, hss.edst, hdr, &hdrlen);

    /* should use negotiated values - min() of primary/secondary */    
    hsp.max_info_tx = hss.cfg.max_info_tx;  
    hsp.max_info_rx = hss.cfg.max_info_rx;
    hsp.window_tx = 1;
    hsp.window_rx = 1;

    hdlc_fill_snrm_param(param_info, sizeof(param_info), &rsplen, &hsp);
    rc = hdlc_send_frame(hdr, param_info, rsplen);

    dlog(LOG_DEBUG, "SNRM-UA response rc %d", rc);

    /* Send / Receive sequence numbers are reset to 0 */
    hss.vr = 0;
    hss.vs = 0;
    hss.vr_ack = 0;
    hss.vs_ack = 0;

    return 0;
 
}


static int 
hdlcs_disc(void)
{
    uint8_t hdr[HDLC_HDR_SIZE];
    int hdrlen;
    int rc;    

    hss.state = HSS_DISC;
            
    dlog(LOG_DEBUG, "disconnecting");

    /* respond with UA */
    hdlc_hdr(0, hdlc_control(HDLC_UA, 1), hss.esrc, hss.edst, hdr, &hdrlen);
    rc = hdlc_send_frame(hdr, NULL, 0);
    return 0;
}

static int
hdlcs_i(struct mbuf *d)
{
    uint8_t hdr[HDLC_HDR_SIZE];
    int hdrlen;
    int rc;   

    ddump(LOG_DEBUG, "Recv I frame", d->data, d->len);
   
    if (hss.icb) {
        /* hand incoming data to registered callback */
        dlog(LOG_ERR, "data CB not supported");
    }
    else if (1) {
        /* make data available ro hdlcs_read() */

        /* segmentation not supported - this is the first and final element */
        hss.recv = d;
        hss.r_complete = 1;
    }
    else {
        /* if there was segmentation, we'd send back an RR to ack */
        hdlc_hdr(0, hdlc_control_rr(hss.vr, 1), hss.esrc, hss.edst, 
                                                hdr, &hdrlen);
        rc = hdlc_send_frame(hdr, NULL, 0);
        if (rc) {
            dlog(LOG_ERR, "I ack RR rc %d", rc);
        }
    }

    return 0;
}



int 
hdlcs_rr(void)
{
    uint8_t hdr[HDLC_HDR_SIZE];
    int hdrlen;

    if (!pending_rsp) {
        dlog(LOG_DEBUG, "respond to RR with RR");
        hdlc_hdr(0, hdlc_control_rr(hss.vr, 1), hss.esrc, hss.edst, hdr, &hdrlen);
        hdlc_send_frame(hdr, NULL, 0);
    }
    else {

        dlog(LOG_DEBUG, "Resending frame");
        hdlcs_write(pending_rsp->m_data, pending_rsp->m_pktlen);

        /* CoAP will also send app confirm */
        /* if not (and there is no data), proxy should send RR to confirm */
    }

    return 0;
}


static int 
hdlcs_dm(void)
{    
    uint8_t hdr[HDLC_HDR_SIZE];
    int hdrlen;

    /* Disconnected Mode response */
    dlog(LOG_WARNING, "request recv'd in disconnected mode");
    
    hdlc_hdr(0, hdlc_control(HDLC_DM, 1), hss.esrc, hss.edst, hdr, &hdrlen);
    hdlc_send_frame(hdr, NULL, 0);

    return 0;

}


static int
hdlcs_frmr(void)
{
    uint8_t hdr[HDLC_HDR_SIZE];
    int hdrlen;
    
    /* Frame Reject response */
    dlog(LOG_WARNING, "error - frame rejected");

    hdlc_hdr(0, hdlc_control(HDLC_FRMR, 1), hss.esrc, hss.edst, hdr, &hdrlen);
    hdlc_send_frame(hdr, NULL, 0);

    return 0;

}
