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
 *
 * HDLC implementation for reliable local connections.
 * 
 * The following choices were made HDLC implemetation:
 * - unbalanced connection-mode data link operation 
 * - two-way alternate data transfer
 * - use frame format type 3
 * - non-basic frame format transparency
 *
 * The frame parser and state machine implemenation below reflects these choices
 *
 *
 */


#include <arduino.h>

#include "hdlc.h"
#include "bufutil.h"
#include "crc_xmodem.h"
#include "log.h"
#include "coapsensorobs.h"

#define HDLC_SINGLE_BYTE_ADDR_ONLY

// This number is fixed for the Milli Arduino Shield
#define UART_BAUD_RATE 38400

// The max payload size
static uint32_t max_payload_size = 0;

// Pointer to Serial console and UART
static HardwareSerial * pU;
#define uart (*pU)
void hdlc_init( HardwareSerial * pUART, uint32_t max_info_len )
{
	// Set pointer to UART object
	pU = pUART;

	// Set baud rate for the mShield UART
	// NOTE: This baud rate is fixed and cannot be changed
	uart.begin(UART_BAUD_RATE);
	
	// Set the max payload size
	max_payload_size = max_info_len;

} // hdlc_set_serial

struct hdlcstat hdlc_stats;

/* HDLC Frame Control field encoders/decoders 
 *
 */


uint8_t
hdlc_control_i(uint8_t nr, uint8_t ns, uint8_t pf)
{
    return ((nr & 0x07) << 5) | ((pf  & 0x01) << 4) | ((ns & 0x07) << 1);
}    

uint8_t
hdlc_control_rr(uint8_t nr, uint8_t pf)
{
   return ((nr & 0x07) << 5) | ((pf & 0x01) << 4) | 0x01;
}
#if 0
static uint8_t
hdlc_control_rnr(uint8_t nr, uint8_t pf)
{
   return ((nr & 0x07) << 5) | ((pf & 0x01) << 4) | 0x05;
}
#endif

/* Control field is a single byte in the frame,
 * Extending the range of this encoder function to allow it 
 * to return an error code as well.
 * return -1 for error, 0..255 encoded control byte 
 */
#define HDLC_FC_INVALID (-1)
int16_t
hdlc_control(uint8_t type, uint8_t pf)
{
    uint8_t control;
    
    switch (type) {        
        case HDLC_SNRM: control = 0x83; break;
        case HDLC_DISC: control = 0x43; break;
        case HDLC_UA:   control = 0x63; break;
        case HDLC_DM:   control = 0x0F; break;
        case HDLC_FRMR: control = 0x87; break;
        case HDLC_UI:   control = 0x03; break;
        default:
            return HDLC_FC_INVALID;
    }

    if (pf) {
        control |= 0x10;
    }
    return control;
}


int
hdlc_parse_control(uint8_t ctrl, struct hdlc_ctrl *hc) 
                 //  uint8_t *type, uint8_t *pf, uint8_t *nr, uint8_t *ns)
{
    uint8_t c2;
    int rc = 0;

    memset(hc, 0, sizeof (*hc));

    if ((ctrl & 1) == 0) {              /* I frame */
        hc->type = HDLC_I;
        hc->nr = (ctrl & 0xE0) >> 5;
        hc->pf = (ctrl & 0x10) >> 4;
        hc->ns = (ctrl & 0x0E) >> 1;
    }
    else if ((ctrl & 0x0F) == 1) {      /* RR frame */
        hc->type = HDLC_RR;
        hc->nr = (ctrl & 0xE0) >> 5;
        hc->pf = (ctrl & 0x10) >> 4;
    }            
    else if ((ctrl & 0x0F) == 5) {      /* RNR frame */
        hc->type = HDLC_RNR;
        hc->nr = (ctrl & 0xE0) >> 5;
        hc->pf  = (ctrl & 0x10) >> 4;
    } else {                            /* the rest w/o sequence numbers */
        hc->pf = (ctrl & 0x10) >> 4;
        c2 = ctrl & 0xEF;
        switch (c2) {
            case 0x83:  hc->type = HDLC_SNRM;  break;
            case 0x43:  hc->type = HDLC_DISC;  break;
            case 0x63:  hc->type = HDLC_UA;    break;
            case 0x0F:  hc->type = HDLC_DM;    break;
            case 0x87:  hc->type = HDLC_FRMR;  break;
            case 0x03:  hc->type = HDLC_UI;    break;
            default:
                rc = 1;     /* Unknown frame - error */
        }
    }
    dlog(LOG_DEBUG, "frame type: 0x%x", hc->type);

    return rc;
}


/* addresses are encoded per HDLC address extension definition 
 * the u32 value is trimmed when inserted into frame header 
 * Limited to single byte encoding only
*/
uint8_t
hdlc_addr_encode(uint32_t address)
{    
    return ((address & 0x7F) << 1) | 1;
}

/* Pick up variable length address from frame, place it into a u32
 * 
 * Returns 1 - parse error or incomplete input
 *         0 - no error
 *
 *        *al - number of bytes consumed in processing
 *              0 means input was found incomplete
 */

int
hdlc_parse_addr(const uint8_t *buf, int len, uint32_t *eaddr, int *al)
{
    *eaddr = 0;
    *al = 0;
    
    if ((len >= 1) &&(buf[0] & 1)) {
        memcpy(eaddr, buf, 1);
        *al = 1;               
    } 
    else {
        /* invalid format _or_ incomplete data */
        goto err;
    }
    
    return 0;
err:
    return 1;
}

/* Encode HDLC header, ready to send, assuming no info field.
 * Frame length and FCS will be updated prior to sending if an info
 * field is attached to the frame
 * addresses are encoded, ready to use
 * HDLC Frame Type 3 support only as noted at in the file comments
 *
 */
int
hdlc_hdr(int segment, int16_t fcontrol, uint32_t dst, uint32_t src,
    uint8_t *buf, int *hdrlen)
{
    uint8_t *b = buf;
    uint16_t hcs;
 
    /* safety checks - fcontrol is valid, src/dst <= 7f */
    if (fcontrol == HDLC_FC_INVALID || dst > 0x7f || src > 0x7f) {
        goto err;
    }
    
    /* Fixed addressing mode - header size is fixed */
    *hdrlen = HDLC_HDR_SIZE;   /* incl. two bytes of HCS */ 
    buf_wbe16(buf, 0, 0xA000 | (segment ? 0x0800 : 0) | *hdrlen);
    b += 2;

    /* Limited to single byte addresses only */
    *b++ = dst;     //hdlc_addr_encode(dst);
    *b++ = src;     //hdlc_addr_encode(src);
    *b++ = fcontrol;

    hcs = crc16(crc16_init(), buf, *hdrlen - 2);
    buf_wle16(b, 0, ~hcs);
   
    return 0;
err:
    return 1;
}

/* Generate updated frame header and 2 byte FCS ready to append 
 * at the tail of info
 */
int
hdlc_frm_add_info(const uint8_t *hdr, uint8_t *fhdr, 
    const uint8_t *info, int infolen, uint8_t *fmfcs)
{
    int fmt, hdrlen;
    uint16_t hcs, fcs;
    
    fmt = buf_be16(hdr, 0);
    hdrlen = fmt & 0x07FF;
    fmt = fmt & 0xF800;

    if (hdrlen > HDLC_HDR_MAX) {
        return 1;
    }

    memcpy(fhdr, hdr, hdrlen);

    /* Modify length value (adding infolen + 2(for FCS) and HCS */
    if (infolen > 0) {           
        buf_wbe16(fhdr, 0, fmt | (hdrlen + infolen + 2));
        hcs = crc16(crc16_init(), fhdr, hdrlen - 2);
        buf_wle16(fhdr, hdrlen - 2, ~hcs);

        /* Calculate separate FCS */
        fcs = crc16(crc16_init(), fhdr, hdrlen);
        fcs = crc16(fcs, info, infolen);
        /* final byte order - ready to be appended to frame */
        buf_wle16(fmfcs, 0, ~fcs);
    }
    return 0;

}

#define HDLC_PARSE_SHORT    (1)
#define HDLC_PARSE_HDR_ERR  (2)

int
hdlc_parse_hdr(struct hdlc_hdr_fields *hh, const uint8_t *buf, int buflen)
{
    int dlen, slen;
    int rc;

    if (buflen < HDLC_HDR_SIZE) { /* don't bother parsing, need more */
        return HDLC_PARSE_SHORT;
    }

    /* Confirm type 3 */
    if ((buf[0] & 0xF0) != 0xA0) {
        return HDLC_PARSE_HDR_ERR;
    }
    /* segment */
    hh->segment = buf[0] & 0x08;
    hh->framelen = buf_be16(buf, 0) & 0x07FF;

    /* try parsing dst/src addresses - these are the only variable len fields */
    rc = hdlc_parse_addr(buf + 2, buflen- 2, &hh->dst, &dlen);
    if (rc || dlen < 1) {
        goto err;
    }
    rc = hdlc_parse_addr(buf + 2 + dlen, buflen - 2 - dlen, &hh->src, &slen);
    if (rc || slen < 1) {
        goto err;
    }


    /* header size is now known - check HCS next */                    
    hh->hdrlen = 2 + dlen + slen + 1 + 2;

    /* grab control field */
    hh->control = buf[2 + dlen + slen];

    /* validate HCS */
    if (crc16_validate(buf, hh->hdrlen)) {
        goto err;
    }

    if (hh->framelen > hh->hdrlen + 2) {
        hh->infolen = hh->framelen - hh->hdrlen - 2;
    } else {
        hh->infolen = 0;
    }

    return 0;

err:
    return HDLC_PARSE_HDR_ERR;

}


#ifndef min
#define min(a,b)    ((a) < (b) ? (a) : (b))
#endif


/* Fill param negotiation information IEC 62056-46 6.4.4.4.3.2 */
int
hdlc_fill_snrm_param(uint8_t *buf, uint32_t buflen, uint32_t *rsplen,
                     struct hdlc_snrm_params *hsp){

    /* Based on example in IEC doc*/
    uint8_t param_template[] = {0x81, 0x80, 0x14,
                                0x05, 0x02, 0x01, 0x00,
                                0x06, 0x02, 0x01, 0x00,
                                0x07, 0x04, 0x00, 0x00, 0x00, 0x01,
                                0x08, 0x04, 0x00, 0x00, 0x00, 0x01};

    if (!buf || buflen < sizeof(param_template)) {
        return 1;
    }

    memcpy(buf, param_template, sizeof(param_template));

    buf_wbe16(buf, 5, hsp->max_info_tx);
    buf_wbe16(buf, 9, hsp->max_info_rx);

    buf_wbe32(buf, 13, hsp->window_tx);
    buf_wbe32(buf, 19, hsp->window_rx);

    *rsplen = sizeof(param_template);

    return 0;
}

/* Parse param tlv rsp.
 * Example:
 * 81 80 14 05 02 00 40 06 02 00 40 07 04 00 00 00 07 08 04 00 00 00 01 */
int
hdlc_parse_snrm_param(const uint8_t *buf, uint32_t buflen, struct hdlc_snrm_params *hsp) {

    uint32_t index = 3;
    uint8_t grplen;
    uint8_t state = 0, type = 0, len = 0;
    uint32_t value = 0;

    /* Make sure header is there (fmtid/grpid/grplen) */
    if (!hsp || !buf || buflen < 3) {
        return 1;
    }

    /* Check fmt id, grp id */
    if (buf[0] != 0x81 || buf[1] != 0x80) {
        return 1;
    }

    grplen = buf[2];

    /* make sure length matches */
    if (grplen < buflen - 3) {
        return 1;
    }

    while (grplen--) {
        switch (state) {
        case 0: /* type */
            type = buf[index++];
            state = 1;
            break;
        case 1: /* len */
            len = buf[index++];
            if (!len || len > 4) {
                return 1;
            }
            value = 0;
            state = 2;
            break;
        case 2: /* value */
            len--;
            value |= (uint32_t)buf[index++] << ((len)*8);

            if (!len) {
                /* Expected types */
                if (type == 5) {
                    hsp->max_info_rx = value;
                } else if (type == 6) {
                    hsp->max_info_tx = value;
                } else if (type == 7) {
                    hsp->window_rx = value;
                } else if (type == 8) {
                    hsp->window_tx = value;
                } else {
                    return 1;
                }
                state = 0;
            }
            break;
        }
    }
    return 0;
}


/*
 *****************************************************************************
 *****************************************************************************
 
    UART Interface --- driver function implemented using POSIX file ops.

 *****************************************************************************
 *****************************************************************************
 */

int hdlc_send_frame( const uint8_t *hdr, const uint8_t *info, int infolen )
{
    uint8_t fhdr[HDLC_HDR_MAX];
    uint8_t fs = HDLC_FLAG;
    uint8_t fcs[2];    
    int rc;
    int i;

    /* attach info if present */
    if (info && infolen > 0) {
        if ((rc = hdlc_frm_add_info(hdr, fhdr, info, infolen, fcs))) {
            return -1;
        }
        hdr = fhdr;
    }

	// Log
    log_msg("HDLC send frame", hdr, HDLC_HDR_SIZE, 0);

	// Send frame delimiter
    uart.write(fs);

	// Send header
    /* TODO: Is this a problem on Arduino? */
	/* Need to know why the first char is dropped on uart */
	rc = uart.write( hdr, HDLC_HDR_SIZE );
    if (rc != HDLC_HDR_SIZE) 
	{
		dlog(LOG_DEBUG, "Error: hdlc_send_frame() did not send %d bytes as required\n", HDLC_HDR_SIZE );
		return -1;
    }
   
    if (info && infolen > 0) 
	{
		// Write payload info
        rc = uart.write(info, infolen);
		if (rc != infolen) 
		{
			dlog(LOG_DEBUG, "Error: hdlc_send_frame() did not send %d bytes as required\n", infolen );
			return -1;
		}

		// Write CRC-16
        rc = uart.write(fcs, HDLC_CRC_SIZE);
		if (rc != HDLC_CRC_SIZE) 
		{
			dlog(LOG_DEBUG, "Error: hdlc_send_frame() did not send %d bytes as required\n", HDLC_CRC_SIZE );
			return -1;
		}

        log_msg(NULL, info, infolen, 0);    
        log_msg(NULL, fcs, HDLC_CRC_SIZE, 0);            

    }

    /* closing with FS.  Not shown in log */
    uart.write(fs);
    log_msg(NULL, NULL, 0, 1);  /* EOL */

    return 0;
}

#define FRAME_FLAG          (1)
#define FRAME_HDR           (2)
#define FRAME_INFO          (3)
#define FRAME_CLOSE_FLAG    (4)
#define FRAME_ERR_FLUSH     (5)

#define HDLC_INFO_MAX   (256)


struct hdlcux {
    /* store full frame, header (incl. HCS), info and FCS */
    uint8_t h_frame[HDLC_HDR_MAX + HDLC_INFO_MAX + 2];
    uint8_t h_infoidx; /* fixed header format, this is constant */
    uint16_t h_infolen;
};

/* Parse header, return OK/Error and the number of bytes still needed to 
 * complete the incoming header
 */
static int hu_hdlc_parse_hdr(const uint8_t *hdr, int datalen, int *need)
{
    int dstlen, srclen, hdrlen;
#ifndef HDLC_SINGLE_BYTE_ADDR_ONLY    
    int i;
#endif

    dstlen = srclen = 0;

    if (datalen < HDLC_HDR_SIZE) {
        goto err;
    }

    if ((hdr[0] & 0xF0) != 0xA0) {  /* only Type 3 is supported */
        //++hustats.hs_frame_type;
        goto err;
    }
    
#ifndef HDLC_SINGLE_BYTE_ADDR_ONLY
    /* establish dst length */
    for (i = 0; i < 4; i++) {
        if (hdr[2 + i] & 0x01) {
            dstlen = i + 1;
            break;
        }
    }
    if (dstlen == 0) {
        //++hustats.hs_hdr_dstlen;
        goto err;        
    }

    /* we may have enough data for examining the src fiels now */
    for (i = 0; i < (datalen - 2 - dstlen); i++) {
        if (hdr[2 + dstlen + i] & 0x01) {
            srclen = i + 1;
            break;
        }
        if (i == 3) {
           /* checked the max 4 bytes for a well formed add - error now */
            //++hustats.hs_hdr_srclen;
            goto err;  
        }
    }

    if (srclen == 0) {
        /* reached end of buffer while parsing src - get 2 more bytes 
         * if nothing else, the hcs bytes are still coming
         * if the header is corrupt, we're likely to get a timeout/HCS err
         */             
        *need = 2;
        goto done;
    }

#else
    /* FIX LENGTHS - 1 byte addresses */
    srclen = 1;
    dstlen = 1;

#endif

    /* did we get HCS (which may turn out to be FCS) */
    hdrlen = 2 + dstlen + srclen + 1 + 2;
    if (datalen >= hdrlen) {
        if (crc16_validate(hdr, hdrlen)) {
            //++hustats.hs_hcs_err;
            goto err;
        }
    } else {
        *need = 1;
        goto done;
    }

done:
    return 0;

err:
    return 1;
}


// Get the payload size
#define PARSE_ERR (-1)
static int hu_hdlc_parse_infolen( const uint8_t *hdr, int hdrlen, uint16_t *infolen )
{

    /* frame length field of 0 (scan for next flag byte) is not supported */
    uint16_t frmlen;
    uint16_t paylen;

    /* Frame type 3, 11 bit length field */
    frmlen = buf_be16(hdr, 0) & 0x07FF;
	
	/* Make sure the frame length is not shorter than the header */
	if ( frmlen < hdrlen )
	{
		return PARSE_ERR;
		
	} // if
	
	/* Compute payload length */
	paylen = frmlen - hdrlen;

	// If info field is present, check the payload length
    if ( paylen && ( paylen < 3 ) )
	{
        /* if the info field is present, the frame must contain at least
         * - the header
         * - info field (1 byte minimum)
         * - separate HCS/FCS (2 bytes additional)
         */
        return PARSE_ERR;
		
    } // if

    /* The returned value includes the 2 bytes of FCS */
    *infolen = paylen;
	return 0;
	
} // hu_hdlc_parse_infolen



struct hdlcu_stats {
    uint32_t hs_ipkts;
    uint32_t hs_opkts;
    uint32_t hs_open;
    uint32_t hs_close;
    uint32_t hs_open_busy;
    uint32_t hs_send_busy;
    uint32_t hs_frame_type;
    uint32_t hs_hdr_dstlen;
    uint32_t hs_hdr_srclen;
    uint32_t hs_hcs_err;
    uint32_t hs_fcs_err;
    uint32_t hs_nomem;
    uint32_t hs_frm_start;
    uint32_t hs_discard;
};

struct hdlcu_stats hustats;


struct hdlcu_ctx {
    uint8_t     hu_state;       /* frame processing state */
    int         hu_pend;        /* number of bytes needed to progress */
    int         hu_frmlen;      /* expected length of incoming frame */
    int         hu_hdrlen;      /* accumulated so far */

    struct hdlcux   hux;        /* current frame being processed */
};
struct hdlcu_ctx hctx;

void print_hctx_state()
{
	char buffer[256];
	sprintf( buffer, "hctx.hu_state: %d", hctx.hu_state );
    dlog( LOG_INFO, buffer );
}

void print_hctx_pend()
{
	char buffer[256];
	sprintf( buffer, "hctx.hu_pend: %d", hctx.hu_pend );
    dlog( LOG_INFO, buffer );
}

#define HDLC_FRAME_BASE         (0)
#define HDLC_FRAME_HDR          (1)
#define HDLC_FRAME_INFO         (2)
#define HDLC_FRAME_CLOSE_FLAG   (3)
#define HDLC_FRAME_ERR_FLUSH    (4)
#define HDLC_FRAME_END			(5)

// UART receive buffer
uint8_t UART_Buf[UART_MAX_BUF_LEN];

// Count the number of received frames
static int hframerecv;

// Sleep for 1 ms while waiting for a frame to arrive on UART
#define MS_SLEEP				(1)

// Receive an HDLC frame
int hdlc_rx( uint8_t *hdr, uint8_t *info, int framesz, int hdlc_frame_timeout )
{
	char buffer[256];
    uint32_t cnt;
	int rc;
	boolean obs_flag;
    float elapsed;
	float timeout;
	uint8_t * pHdr;
	uint8_t * pPayload;
	uint16_t rx_len = 0;
	uint16_t frame_len = 0;
	struct hdlcux * pHUX = &hctx.hux;

    memset( pHUX, 0x0, sizeof(hctx.hux) );

	// Read UART for maximum 200 ms
	uart.setTimeout(READ_BUF_TIMEOUT);	 

	// Wait for incoming HDLC frame
	elapsed = 0.0;
	timeout = (float) hdlc_frame_timeout;
	while( elapsed < timeout ) 
	{
		// Check if there is nothing at the UART
		if (!uart.available())
		{
			// Check if it is time to send Observe response message
			// The function call returns a flag that determines if Observe is turned on
			obs_flag = do_observe();
			
#if defined(ARDUINO_ARCH_SAMD)
			if (obs_flag)
			{
				// The reading of the time on SAMD takes about 5.5 ms
				// Adjust the variable accordingly
				elapsed += 5.5;
				continue;
			}
#endif
			// Sleep for 1 ms
			delay(MS_SLEEP);
			elapsed++;
			continue;
			
		} // if
		
		// Read the HDLC frame until time-out
		cnt = uart.readBytes( UART_Buf, UART_MAX_BUF_LEN );
		sprintf( buffer, "readBytes() count: %d", cnt );
		dlog( LOG_INFO, buffer );
		capture_dump( UART_Buf, cnt );
		
		// Check if we received more bytes than there is space for in the receive buffer
		if ( cnt > UART_MAX_BUF_LEN )
		{
			// This should never happen as the readBytes method above already sets the limit
			dlog( LOG_DEBUG, "The UART receive buffer has overflown!" );
			sprintf( buffer, "We read %d bytes and the max is %d bytes.", cnt, UART_MAX_BUF_LEN );
			dlog( LOG_DEBUG, buffer );
			
		} // if

		// Check for HDLC frame delimiters
		hctx.hu_state = HDLC_FRAME_BASE;
		if (( UART_Buf[0] != HDLC_FLAG ) || ( UART_Buf[cnt-1] != HDLC_FLAG ))
		{
            ++hustats.hs_discard;  
			dlog( LOG_DEBUG, "Missing HDLC flag(s)" );
			return 0;
			
		} // if
		
		/* Number of bytes received excluding the two frame delimiters */
		rx_len = cnt - 2;
		
		// Parse the header
		pHdr = &UART_Buf[1];
		rc = hu_hdlc_parse_hdr( pHdr, HDLC_HDR_SIZE, &hctx.hu_pend );
		if (rc) 
		{
			/* header parsing error - need to flush */
			hctx.hu_state = HDLC_FRAME_ERR_FLUSH;
			dlog( LOG_DEBUG, "Bad hdr - flush" );
			return 0;
			
		} // if

		/* TODO: How should this be handled? */
		if ( hctx.hu_pend == 2 ) 
		{
			dlog( LOG_DEBUG, "hctx.hu_pend == 2" );
			return 0;
			
		} // if

		/* Header complete - always, working with fixed hdr size */
		/* Payload, if any, starts after the header */
		hctx.hu_state = HDLC_FRAME_HDR;
		pHUX->h_infoidx = HDLC_HDR_SIZE;

		// Get payload size
		rc = hu_hdlc_parse_infolen( pHdr, HDLC_HDR_SIZE, &pHUX->h_infolen );
		if (rc)
		{
			hctx.hu_state = HDLC_FRAME_ERR_FLUSH;
			dlog( LOG_DEBUG, "bad infolen - flush" );
		
		} // if
		
		// Check if the frame length equals the number of received bytes
		frame_len = pHUX->h_infoidx + pHUX->h_infolen;
		if ( frame_len != rx_len )
		{
			dlog( LOG_DEBUG, "The frame length doesn't match the number of received bytes" );
			print("frame_len: ");	printnum(frame_len);	println("");
			print("rx_len:    ");	printnum(rx_len);		println("");
			return 0;
			
		} // if

		// CRC check
		if ( crc16_validate( pHdr, frame_len )) 
		{
			dlog( LOG_DEBUG, "Discard frame - CRC error" );
			return 0;
		}
		
		/* Return header */
		memcpy( hdr, pHdr, HDLC_HDR_SIZE );

		// Check for payload
		if (pHUX->h_infolen) 
		{
			/* If the payload is present, it can't be zero bytes */
			if ( pHUX->h_infolen <= HDLC_CRC_SIZE ) 
			{
				/* Invalid payload size */
				dlog( LOG_DEBUG, "Discard frame - bad info len" );
				return 0;
				
			} // if

			/* Check if payload is greater than the maximum allowed */
			rx_len = pHUX->h_infolen - HDLC_CRC_SIZE;
			if ( rx_len > max_payload_size )
			{
				dlog( LOG_DEBUG, "The HDLC payload is too large!" );
				sprintf( buffer, "We got %d bytes and the max is %d bytes.", rx_len, max_payload_size );
				dlog( LOG_DEBUG, buffer );
				return 0;
				
			} // if

			// Return payload
			pPayload = pHdr + HDLC_HDR_SIZE;
			memcpy( info, pPayload, rx_len );
			hctx.hu_state = HDLC_FRAME_INFO;
		}
		else 
		{
			dlog( LOG_DEBUG, "Zero infolen" );
			
		} // if-else

		// Increment the receive frame counter
		hframerecv++;
		log_msg( "HDLC recv frame", pHdr, frame_len, 1 );
		return 1;

    } // while

	// Time-out
	return 0;
	
} // hdlc_rx()
