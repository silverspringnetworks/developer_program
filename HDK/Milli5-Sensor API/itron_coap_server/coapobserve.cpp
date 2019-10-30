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
#include "coapsensoruri.h"
#include "coapobserve.h"

/*
 * The main issue is with the client field, since that represents something
 * that is ultimately dereferenced by the caller. Other fields changing
 * mid-stream should be okay, but if a GET disabled observe on a given URI
 * while an observe response is in progress and the client is already freed,
 * we could have a double free, or a free and then a derefrence.
 */

/*
 * For tracking what's being observed. Allows a mapping between the resource
 * (uri) the token associated with the request, and the client's handle.
 * NB Only one client is assumed.
 */
//#define MAX_OBS_URI_LEN     MAX_URI_LEN
//#define MAX_OBSERVERS       4

struct obs_t {
    char uri[MAX_OBS_URI_LEN];  /* Resource being observed */
    uint8_t tkl;                /* Token length and token */
    uint8_t token[8];
    void *client;               /* Opaque client handle */
    char sid[SID_MAX_LEN];      /* sensor id, if present */
};

/*
 * A simple array of observers.
 */
static struct obs_t obs[MAX_OBSERVERS] = { };

/*
 * Find the observe entry in the array specified by the token and the sensor
 * identifier. For now it just finds the matching entries in obs[], but in
 * future we'll need a more sophisticated mapping if multiple clients and/or
 * sensors will be upported concurrently, as tokens are only unique to a given
 * endpoint. The issue is that we don't know what resource is responding unless
 * the sensor was to include it in the options, thus we're relying on the token
 * being globally unique. This would be OK if the proxy generated a unique
 * token when forwarding the original request, but then we'll need a
 * token:token mapping.
 *
 * @param sid: Sensor id (relevant component of URI), part of the key.
 * @param tkl: The token length.
 * @param token: The token to use as the key.
 * @param client: To return the opaque client handle that was provided when the
 * observe was set up.
 * @param nxt: index after the one where the entry was found. Start with 0 on
 * the first call.
 *
 * Returns 0 on success, -1 if not found.
 */
error_t
get_obs_by_sid_tok(const char *sid, uint8_t tkl, const uint8_t *token, 
                  void **client, uint8_t *nxt)
{
    uint8_t i;

    if ((tkl == 0) || (token == NULL) || (*nxt >= MAX_OBSERVERS) || 
            (sid == NULL)) {
        return ERR_INVAL;
    }

    for (i = *nxt; i < MAX_OBSERVERS; i++) {
        if (!memcmp(token, obs[i].token, tkl) && !strcmp(sid, obs[i].sid)) {
            break;
        }
    }
    if (i == MAX_OBSERVERS) {
        return ERR_NO_ENTRY;
    }

    *client = obs[i].client;
    *nxt = i + 1;

    return ERR_OK;
}



/*
 * Find the observe entry in the array specified by the uri. 
 *
 * @param uri: The URI entry to find. Observers watch a resource, defined by a
 * URI.
 * @param tkl: To return the token length.
 * @param token: To return the token associated with the original REQ, which we
 * must use in all RSPs.
 * @param client: To return the opaque client handle that was provided when the
 * obseve was set up.
 * @param nxt: index after the one where the entry was found. Start with 0 on
 * the first call.
 *
 * Returns 0 on success, -1 if not found.
 */
error_t
get_obs_by_uri(const char *uri, uint8_t *tkl, uint8_t *token, void **client, uint8_t *nxt)
{
    uint8_t i;

    if ((uri == NULL) || (uri[0] == '\0') || (*nxt >= MAX_OBSERVERS)) {
        return ERR_INVAL;
    }

    for (i = *nxt; i < MAX_OBSERVERS; i++) {
        if (!strcmp(uri, obs[i].uri)) {
            break;
        }
    }
    if (i == MAX_OBSERVERS) {
        return ERR_NO_ENTRY;
    }

    *tkl = obs[i].tkl;
    memcpy(token, obs[i].token, *tkl);
    *client = obs[i].client;
    *nxt = i + 1;

    return ERR_OK;
}



/*
 *  Get the next observe option value. Values 0 and 1 are reserved for the
 *  initial GET request and cancellation of the request respectively. 24 bits
 *  maximum.
 */
uint32_t
get_obs_val(void)
{
    /*
     * Monotonically increasing observe value.
     */
    static uint32_t obs_val = 2;

    if (obs_val == (1 << 24) - 1) {
        obs_val = 2;
    } else {
        obs_val++;
    }

    return obs_val;
}


/*
 * Given a slot in the observer array, add an entry. Assumes slot within array
 * range!
 * This can only be called by the main (on the NIC, net_mgr) task.
 */
static void
add_obs(int slot, const char *urip, struct coap_msg_ctx *req, void *client)
{
    obs[slot].tkl = req->tkl;
    memcpy(obs[slot].token, req->token, req->tkl);
    strcpy(obs[slot].uri, urip);
    obs[slot].client = client;
    strcpy(obs[slot].sid, req->sid);
}


/*
 * Add or update an entry in the obs observer array, if possible, and return 0.
 * If the array is full, the entry already exists, return -1.
 * TODO: Differentiate between clients based on endpoint info.
 * Assumes the appropriate mechanism (e.g. signal handler) has been set up to
 * make use of the array for sending observe notifications.
 *
 * TODO What to do if an entry with the same token exists? For now should fail
 * to add, but in future client endpoint info should be used to distinguish
 * between entries with the same token.
 *
 * Currently only called from main (net_mgr) task on NIC, so no need for
 * locking.
 */
error_t 
enable_obs(const char *urip, struct coap_msg_ctx *req, void *client)
{
    int i;
    int empty_slot = MAX_OBSERVERS;

    if (urip == NULL)
	{
		goto error;
    }

    /*
     * Find the uri in the array, if present.
     * Add it to the array, with the token.
     */
    if (strlen(urip) >= MAX_OBS_URI_LEN)
	{
        goto error;
    }

    /*
     * If this is a sensor observe request, first ensure that sensor_id:token
     * are unique, because, for now at least, that's all we're going to get
     * from the async packets from the sensor. NB sensor_id is just the unique
     * part of the uri that identifies a request for a sensor.
     * TODO Remove this check and allow multiple entries for a given URI, which
     * would mean multiple endpoints interested in that URI, which is valid.
     */
    if (req->sid[0] != '\0')
	{
        for (i = 0; i < MAX_OBSERVERS; i++) {
            if (!strncmp(req->sid, obs[i].uri, strlen(req->sid)) && 
                !memcmp(req->token, obs[i].token, MAX(req->tkl, obs[i].tkl)))
			{
                dlog(LOG_INFO, "Not adding obs entry for %s, sid:token not unique", urip);
                return ERR_EXISTS;
            }
        }
    }

    /*
     * Now match on URI, which is all we're supposed to do based on the RFC.
     */
    for (i = 0; i < MAX_OBSERVERS; i++) {
        if (!strcmp(urip, obs[i].uri))
		{
            dlog(LOG_INFO, "Not adding obs entry for %s, duplicate.", urip);
            return ERR_EXISTS;
        }
		else if ((obs[i].uri[0] == '\0') && (empty_slot == MAX_OBSERVERS))
		{
            empty_slot = i;
        }
    }
    if (empty_slot < MAX_OBSERVERS)
	{
        add_obs(empty_slot, urip, req, client);
        return ERR_OK;
    }

error:
    return ERR_BAD_DATA;
}



/*
 * Disable observation of the given uri and token.
 * This should be called when:
 *  1. A reset on the interaction identified by the token is received.
 *  2. An explicit GET with observe value == 1 is received.
 *  It's up to the caller to ensure the client handle is freed, which should
 *  happen in the client code.
 *
 * Currently only called from main (net_mgr) task on NIC, so no need for
 * locking. However, as per enable_obs and client pointer.
 */
error_t 
disable_obs(const char *urip, struct coap_msg_ctx *req, void **client, uint8_t force)
{
    int i;

    if (urip == NULL)
	{
        return ERR_INVAL;
    }

    /*
     * Find the uri in the array, and if present, zero the entry.
     */
    for (i = 0; i < MAX_OBSERVERS; i++)
	{
        if (!strcmp(urip, obs[i].uri) && (!memcmp(req->token, obs[i].token, MAX(req->tkl, obs[i].tkl)) || force))
		{
            obs[i].uri[0] = '\0';
            *client = obs[i].client;
            obs[i].client = NULL;
            memset(obs[i].token, 0, sizeof(obs[i].token));
            obs[i].sid[0] = '\0';
            dlog(LOG_INFO, "disable_obs: De-registered URI: %s", urip);
            return ERR_OK;
        }
    }
    return ERR_NO_ENTRY;
}
