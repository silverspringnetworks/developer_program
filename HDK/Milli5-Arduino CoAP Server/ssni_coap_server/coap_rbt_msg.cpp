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
#include "hbuf.h"
#include "errors.h"
#include "exp_coap.h"
#include "arduino_pins.h"
#include "coap_rbt_msg.h"


static const uint8_t rbtput[] = {
	0x50, 0x03, 0xFF, 0xFF,                 /* NON, PUT */
	0xb3, 's', 'y', 's',                    /* Uri-Path */
	0x04, 's', 'n', 's', 'r',               /* Uri-Path */
	0x46, 'e', 'v', '=', 'r', 'b', 't'      /* Uri-Query */
};


// Buffers pointed to are sent to the mnic CoAP proxy by HDLC
extern struct mbuf *pending_rsp;

// CoAP Stats for this IC CoAP server
extern struct coap_stats coap_stats;


/**
 * Send a PUT system reboot event request to the mnic. This is called
 * when the IC reboots, and the intent is that the mnic may have some 
 * action that it wishes to perform in response. e.g. set our time, etc.
 *
 * Given that there are HDLC retries, we're not going to attempt CoAP retries.
 * Could be added later if required.
 */
error_t coap_put_ic_reboot_event(void)
{
	struct mbuf *req;
	uint8_t     *ptr;

	/* Allocate request buffer */
	MGETHDR(req);
	if (!req) 
	{
		coap_stats.no_mbufs++;
		return ERR_NO_MEM;
	}

	/* Assuming plenty of room in mbuf */
	ptr = (uint8_t *)m_append(req, sizeof(rbtput));
	memcpy(ptr, rbtput, sizeof(rbtput));

	/* Send the request to the mnic */
	/* If pending buffer free it - no memory leaks */
	if (pending_rsp)
	{
		m_free(pending_rsp);
	}
	pending_rsp = req;
	dlog(LOG_DEBUG, "Sending reset event to mnic");

	/* Notify mnic of request, wait for 1ms, then high again */
	digitalWrite(MNIC_WAKEUP_PIN, LOW);
	delay(1);
	digitalWrite(MNIC_WAKEUP_PIN, HIGH);

	return ERR_OK;
}