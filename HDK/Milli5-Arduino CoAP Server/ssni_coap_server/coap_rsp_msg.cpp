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
#include "coap_rsp_msg.h"
#include "arduino_time.h"


// Assemble a CoAP response message; {Timestamp,Value,Unit}
error_t rsp_msg( struct mbuf * m, uint8_t *len, uint32_t count, float * reading, const char * unit )
{
    uint8_t 	l;
	char 		rsp_buf[256];
	char		reading_buf[128];
	char		unit_buf[32];
	char * 		p;
	time_t     	epoch;
	uint32_t	ix;
	
	// Create string containing the UNIX epoch
	epoch = get_rtc_epoch();
	sprintf( rsp_buf, "%d", epoch );
	
	// Check if we have a sensor reading
	if (reading)
	{
		// Get each value
		for( ix = 0; ix < count; ix++ )
		{
			// Create string containing reading
			sprintf( reading_buf, ",%.2f", *reading++ );

			// Concatenate the reading
			strcat( rsp_buf, reading_buf );
		}
	}

	// Check if we have a unit
	if (unit)
	{
		// Concatenate the unit
		sprintf( unit_buf, ",%s", unit );
		strcat( rsp_buf, unit_buf );
	}

	// Print message
	dlog( LOG_INFO, rsp_buf );
	
	// Get length
	l = strlen(rsp_buf);
	
	// Allocate memory
	p = (char*) m_append(m, l);
    if (!p) 
    {
        return ERR_NO_MEM;
    }

	// Store output
	strcpy(p,rsp_buf);
	*len = l;
	
    return ERR_OK;
	
} // rsp_msg
