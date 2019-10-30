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

#ifndef INC_COAPURI_H
#define INC_COAPURI_H

#include "errors.h"
#include "coapmsg.h"


// CoAP Server Version Title
#define COAP_SERVER_VERSION_NUMBER "1.4.6"
#define COAP_SERVER_VERSION_STRING "Itron Reference CoAP Server: "

// Related to URI Path
#define CLASSIFIER_MAX_LEN			16
#define DEFAULT_CLASSIFIER			"arduino"

// Longest Uri-Path+Uri-Query we'll handle
#define MAX_URI_LEN					256

//#define L_URI_Q_CFG         "cfg"
//#define L_URI_Q_SENS        "sens"

//* \typedef coap_cb */
typedef error_t (*coap_cb)(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp);

//* \struct coap_cb_reg */
struct coap_cb_reg {
    const char *path;       /* top level Uri-Path */
    coap_cb cb;             /* resource handler for Uri-Path */
    const char *link;       /* CoRE Link Attributes (if any) */
};

/** @brief
 * This structure is opaque to the core sensor code, like haiku. It is to be
 * passed back into coap_local_obs_rsp for looking up the URI and the content
 * format. The code that knows this information and that passes it to the core
 * sensor code is the sensor's registry handler, like crhaiku.
 */
//! \struct sensor_ctx
struct sensor_ctx {
    char        uri[MAX_URI_LEN];   /* URI handled by the sensor. */
    uint8_t     cf;                 /* content format */
};


/**
 * @brief Register CoAP URI
 *
 * @param[in] path URI Path
 * @param[in] cbfunc CoAP Request/Response Handler
 * @param[in] corelink corelink
 * @return error_t
 */
error_t coap_uri_register(const char *path, coap_cb cbfunc, const char *corelink);


/**
 * @brief Initialize CoAP URI registry
 *
 */
void coap_registry_init(void);

/** @brief
 * Call registered handler for given URI in the request
 *
 * @param[in] req CoAP Message request
 * @param[out] rsp CoAP Response
 * @return error_t
 */
error_t coap_s_uri_proc(struct coap_msg_ctx *req, struct coap_msg_ctx *rsp);

#endif /* INC_COAPURI_H */
