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

#ifndef _INC_COAPOBSERVE_H_
#define _INC_COAPOBSERVE_H_

#include "errors.h"
#include "coapmsg.h"

#define COAP_OBS_REG                    0x0
#define COAP_OBS_DEREG                  0x1


/*
 * For tracking what's being observed. Allows a mapping between the resource
 * (uri) the token associated with the request, and the client's handle.
 * NB Only one client is assumed.
 */
#define MAX_OBS_URI_LEN     MAX_URI_LEN
#define MAX_OBSERVERS       4


error_t enable_obs(const char *urip, struct coap_msg_ctx *req, void *client);
error_t disable_obs(const char *urip, struct coap_msg_ctx *req, void **client, 
                uint8_t force);
uint32_t get_obs_val(void);
error_t get_obs_by_uri(const char *uri, uint8_t *tkl, uint8_t *token, void **client,
                   uint8_t *nxt);
error_t get_obs_by_sid_tok(const char *sid, uint8_t tkl, const uint8_t *token, 
                  void **client, uint8_t *nxt);
#endif /* _INC_COAPOBSERVE_H_ */
