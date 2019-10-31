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
 * EchoSensor returns a string with a message count. The sensor uses a default string.
 * The string may be changed via a CoAP PUT on using the "cfg" query string.
 *
 * While not a true sensor (as there is no hardware hooked up) it can be used to get a feel for how
 * notifications flow and when responses get lost. It's a cheap hello world.
 */
#include "EchoSensor.h"


// Sensor state is stored in these variables. These could be persisted to flash.
static char		echostring[32];
static uint32_t echocount;


//////////////////////////////////////////////////////////////////////////
//
// Initialize echo sensor. Callback called by sapi_init_sensor function.
//
//////////////////////////////////////////////////////////////////////////
sapi_error_t echo_init_sensor()
{
	// Set the default echo string and message count
	strcpy(echostring, "Hello World:");
	echocount = 1;
	
	dlog(LOG_DEBUG, "Initialized Echo Sensor");
	return SAPI_ERR_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// Reads the echo sensor. Builds and returns the payload. Callback called on
//  CoAP Get sensor value
//  CoAP Observe notification of sensor value
//
//////////////////////////////////////////////////////////////////////////
sapi_error_t echo_read_sensor(char *payload, uint8_t *len)
{
	// Working buffer
	char count[16];
	
	// General form of producing a CoAP response payload.
	strcpy(payload, echostring);
	sprintf(count, " %d", echocount++);
	strcat(payload, count);
	*len = strlen(payload);
	
	return SAPI_ERR_OK;
}


//////////////////////////////////////////////////////////////////////////
//
// Write sensor configuration. Processes payload sent from a CoAP client. Callback called on
//  CoAP Put configuration value
//
//////////////////////////////////////////////////////////////////////////
sapi_error_t echo_write_cfg(char *payload, uint8_t *len)
{
	// Check query string. We key of "cfg=".
	if (strncmp(payload, "cfg=",4) == 0)
	{
		// Update sensor state.
		strcpy(echostring, &payload[4]);
		echocount = 1;
		
		return SAPI_ERR_OK;
	}
	// Not implemented if unexpected query string.
	else
	{
		return SAPI_ERR_NOT_IMPLEMENTED;
	}
}
