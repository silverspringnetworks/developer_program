/*

Copyright (c) Itron, Inc. 
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
#include <stdarg.h>
#include "log.h"    
#include "arduino_time.h"

extern int verbose;

static int log_level = LOG_DEBUG;

// default LOG_ERROR, -v WARN, -vv NOTICE, -vvv INFO, -vvvv DEBUG

static const char *label[] = {"EMRG", "ALRT", "CRIT", "ERR", "WARN", "NOTE", "INFO", "DEBG"};
static const int numlevels = sizeof (label) / sizeof(label[0]);

// Pointer to Serial USB object used for logging
//static HardwareSerial *pSerMon = NULL;
static PORTABILITY_SERIAL_USB *pSerMon = NULL;
#define SerMon (*pSerMon)
static bool log_enabled = false;


//void log_init(HardwareSerial *pSerial, uint32_t baud, uint32_t log_level)
void log_init(PORTABILITY_SERIAL_USB *pSerial, uint32_t baud, uint32_t log_level)
{
	// Assign pointer used for printing
	pSerMon = pSerial;

	// Start the Native serial port
	SerMon.begin(baud);
 
	// Wait for the port to connect
	//while(!SerMon);
	// Give 16s to allow USB to connect to monitor.
	log_enabled = false;
	for (int indx=0 ; indx < 8 ; indx++) {
		if (!SerMon){
			delay(2000);
		} else {
			log_enabled = true;
			break;
		}
	}
	
	// Set the logging level
	dlog_level(log_level);

} // log_init


void dlog_level(int level)
{
    /* force level bounds */
    level = level >= numlevels ? numlevels - 1 : level;
    level = level < 0 ? 0 : level;

    log_level = level;
} // dlog_level


void dlog(int level, const char *format, ...)
{
    va_list args;
	char buffer[PRINTF_LEN];
	
	// Is logging enabled?
	if (!log_enabled)
	{
		return;
	}
   
    // Check debug log
    if (level > log_level) 
	{
        return;
    }

	// Print time
	print_log_time();

	// Print to serial port using the format
	va_start( args, format );
	vsprintf( buffer,format, args );
	SerMon.println(buffer);
	va_end(args);

} // dlog


void ddump(int level, const char *label, const void *data, int datalen)
{
    const uint8_t *b = (const uint8_t *) data;
	char buffer[PRINTF_LEN];
    int i;
    
    // Is logging enabled?
    if (!log_enabled)
    {
	    return;
    }

    if (level > log_level) 
	{
        return;
    }

	// Print time
	print_log_time();

    if (label) 
	{
		sprintf( buffer, "%s:", label );
        SerMon.print(buffer);
    }

    for(i = 0; i < datalen; i++) 
	{
		sprintf( buffer, " %02x", b[i] );
        SerMon.print(buffer);
    }
    
    SerMon.println("");

} // ddump


void log_msg(const char *label, const void *data, int datalen, int eol)
{
    static char llabel[64];
    static int llen;
    static uint8_t line[256];
    
    // Is logging enabled?
    if (!log_enabled)
    {
	    return;
    }

    if ( LOG_DEBUG > log_level ) 
	{
        return;
    }

    if ((!eol || llen) && (llen + datalen < sizeof(line))) {
        /* buffer if we can */
        memcpy(line + llen, data, datalen);
        llen += datalen;
        datalen = 0;    /* consumed */
        if (label) {
            strncpy(llabel, label, sizeof(llabel)-1);
        }
    }

    if (eol || datalen) {
        /* flush */
        if (llen) {
            ddump(LOG_DEBUG, llabel, line, llen);
            llabel[0] = 0;
            llen = 0;
        }
        
        if (datalen) {
            ddump(LOG_DEBUG, label, data, datalen);
        }

    } // if

} // log_msg


void print( const char * buf )
{
	// Is logging enabled?
	if (!log_enabled)
	{
		return;
	}
	
    if ( LOG_DEBUG > log_level ) 
	{
        return;
    }

	SerMon.print(buf);
	
} // print


void println( const char * buf )
{
	// Is logging enabled?
	if (!log_enabled)
	{
		return;
	}
	
    if ( LOG_DEBUG > log_level ) 
	{
        return;
    }

	SerMon.println(buf);
	
} // println


void printnum( int n )
{
	// Is logging enabled?
	if (!log_enabled)
	{
		return;
	}
	
    if ( LOG_DEBUG > log_level ) 
	{
        return;
    }

	SerMon.print(n);
	
} // println


uint8_t capture_buf[1024];
uint16_t cap_count = 0;


void capture( uint8_t ch )
{
	capture_buf[cap_count++] = ch;
	
} // capture


void capture_dump( uint8_t * p, int count )
{
	char str[6];
    uint8_t ch;
	uint16_t ix;
	
	// Is logging enabled?
	if (!log_enabled)
	{
		return;
	}

    // Check debug log
    if ( LOG_DEBUG > log_level ) 
	{
        return;
    }
	
	if (!p)
	{
		p = &capture_buf[0];
	}

	if ( !count && ( cap_count >= 0 ))
	{
		count = cap_count;
		
	}
	
	SerMon.println("======================================================");
	for( ix = 0; ix < count-1; ix++ )
	{
		ch = p[ix];
		sprintf( str, "%02x,", ch );
		SerMon.print(str);

	} // for
	sprintf( str, "%02x", p[ix] );
	SerMon.println(str);
	SerMon.println("======================================================");

	// Reset the count
	cap_count = 0;

} // capture_dump


// Function to return the amount of Free Ram.
extern "C" char *sbrk(int i);
int free_ram()
{
	char stack_dummy = 0;
	return &stack_dummy - sbrk(0);
}

