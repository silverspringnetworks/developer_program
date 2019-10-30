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

#ifndef _INC_HDLC_SECONDARY_H_
#define _INC_HDLC_SECONDARY_H_

#include <HardwareSerial.h>
#include "errors.h"

/* Open HDLCS connection */
error_t hdlcs_open( HardwareSerial * pUART, uint32_t timeout_ms, uint32_t max_hdlc_info_len );

/* shut down state machine */
int hdlcs_close(void);

/* process pending transaction */
int hdlcs_run(void);

/* get incoming reassembled app layer data */
struct mbuf;
struct mbuf *hdlcs_read(void);
/* hand outgoing app layer data to HDLC */
int hdlcs_write(const void *data, uint16_t len);

/* Exposed to clients so they can signal no response. */
int hdlcs_rr(void);

/* app may register for callbacks on info */
typedef int (*hdlcs_data_handler)(struct mbuf *h, int op);
void hdlcs_data_register(hdlcs_data_handler f);

/*
 * @brief Check if we are in connected state
 *
 */
boolean hdlcs_is_connected();

#endif /* _INC_HDLC_SECONDARY_H_ */

