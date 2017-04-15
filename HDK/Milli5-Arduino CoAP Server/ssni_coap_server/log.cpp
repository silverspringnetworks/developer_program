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
#include <stdarg.h>
#include "log.h"    
#include "arduino_time.h"

extern int verbose;

static int log_level = LOG_DEBUG;

// default LOG_ERROR, -v WARN, -vv NOTICE, -vvv INFO, -vvvv DEBUG

static const char *label[] = {"EMRG", "ALRT", "CRIT", "ERR", "WARN", 
                              "NOTE", "INFO", "DEBG"};
static const int numlevels = sizeof (label) / sizeof(label[0]);


/* Init logging */
void log_init( Serial_ * pSerial, uint32_t baud, boolean log_level )
{
	// pS and Serial is defined in log.h
	// This is the only location pS is assigned a value
	pS = pSerial;
	Serial.begin(baud);
	delay(4000);
	//while(!Serial);
	
	// Turn logging on or off
	if (log_level)
	{
		// Turn on logging
		dlog_level(LOG_DEBUG);
	}
	else
	{
		// Turn off logging
		dlog_level(-1);
		
	} // if

} // log_init

/**
* @brief
* Get pointer to Serial
* NOTE: log_init() must be called before this function
*
* @return Serial_ pointer to Serial object used for printing to console
*
*/
Serial_ * log_get_serial()
{
	assert(pS);
	return pS;
	
} // log_get_serial

void dlog_level(int level)
{
    /* force level bounds */
    level = level >= numlevels ? numlevels - 1 : level;
    level = level < 0 ? 0 : level;

    log_level = level;
}

void dlog(int level, const char *format, ...)
{
    va_list args;
	char buffer[PRINTF_LEN];
   
    // Check debug log
    if (level > log_level) 
	{
        return;
    }

	// Print time
	print_current_time();

	// Print to serial port using the format
	va_start( args, format );
	vsprintf( buffer,format, args );
	Serial.println(buffer);
	va_end(args);

} // dlog

void ddump(int level, const char *label, const void *data, int datalen)
{
    const uint8_t *b = (const uint8_t *) data;
	char buffer[PRINTF_LEN];
    int i;

    if (level > log_level) 
	{
        return;
    }

	// Print time
	print_current_time();

    if (label) 
	{
		sprintf( buffer, "%s:", label );
        Serial.print(buffer);
    }

    for(i = 0; i < datalen; i++) 
	{
		sprintf( buffer, " %02x", b[i] );
        Serial.print(buffer);
    }
    
    Serial.println("");

} // ddump


void log_msg(const char *label, const void *data, int datalen, int eol)
{
    static char llabel[64];
    static int llen;
    static uint8_t line[256];

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


uint8_t capture_buf[1024];
uint16_t cap_count = 0;

void print_number( const char * label, int d )
{
	char str[256];
	
    if ( LOG_DEBUG > log_level ) 
	{
        return;
    }

	sprintf( str, "%s", label );
	Serial.print(str);

	sprintf( str, "%d", d );
	Serial.println(str);
	
} // print_number



void print_buf( const char * buf )
{
    if ( LOG_DEBUG > log_level ) 
	{
        return;
    }

	Serial.println(buf);
	
} // print_buf

void capture( uint8_t ch )
{
	capture_buf[cap_count++] = ch;
	
} // capture

void capture_dump( uint8_t * p, int count )
{
	char str[6];
    uint8_t ch;
	uint16_t ix;

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
	
	Serial.println("======================================================");
	for( ix = 0; ix < count-1; ix++ )
	{
		ch = p[ix];
		sprintf( str, "%02x,", ch );
		Serial.print(str);

	} // for
	sprintf( str, "%02x", p[ix] );
	Serial.println(str);
	Serial.println("======================================================");

	// Reset the count
	cap_count = 0;

} // capture_dump

