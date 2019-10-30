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

#ifndef _INCLUDES_H_
#define _INCLUDES_H_

/*
 * Header files and definitions expected to be in every file
 * regardless of architecture, even for tools.
 */

// TODO: What header file should be used instead of endian.h in Arduino?

#include <sys/types.h>
#include <stdint.h>      /* this defines uint32_t, etc */
#include <stdlib.h>
#include <assert.h>


#ifndef min
#define min(a, b) ((a)<(b)?(a):(b))
#endif

#ifndef max
#define max(a, b) ((a)>(b)?(a):(b))
#endif

#ifndef abs
#define abs(v) ((v) < 0 ? ((v) * (-1)) : (v))
#endif

#ifndef roundup2
#define roundup2(x, y) (((x)+((y)-1))&(~((y)-1))) /* if y is powers of two */
#endif

#ifndef rounddown2
#define rounddown2(x, y)    ((x)&(~((y)-1)))   /* if y is powers of two */
#endif

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* mark a parameter as potentially unused, to remove warnings */
#define UNUSED(x)  (void)(x)

/* maybe a better way to do that */
#define unused_t __attribute__((unused))

/* Aligned memory buffers used for pools should use this */
#define aligned_t __attribute__((aligned))

/* Definition for "packed" structures */
#define packed_t __attribute__((packed))

#if defined( DEBUG_IMAGE )
#define DEBUG_ASSERT(x)  assert(x)
#else
#define DEBUG_ASSERT(x)
#endif

#if __STDC_VERSION__ >= 201112L && !defined __cplusplus && !defined __COVERITY__
# define STATIC_ASSERT(x) _Static_assert((x), "Static assertion failed")
#else
# define PASTE(x, y) x##y
# define PASTE2(x, y) PASTE(x, y)
# define STATIC_ASSERT(x) enum { PASTE2(SA, __LINE__) = 1 / (x) }
#endif


#define    bswap64(x) \
    ((uint64_t) \
     ((((x) & 0xff00000000000000ull) >> 56) | \
      (((x) & 0x00ff000000000000ull) >> 40) | \
      (((x) & 0x0000ff0000000000ull) >> 24) | \
      (((x) & 0x000000ff00000000ull) >>  8) | \
      (((x) & 0x00000000ff000000ull) <<  8) | \
      (((x) & 0x0000000000ff0000ull) << 24) | \
      (((x) & 0x000000000000ff00ull) << 40) | \
      (((x) & 0x00000000000000ffull) << 56)))

#define    bswap32(x) \
    ((uint32_t)                   \
    ((((x) & 0xff000000) >> 24) | \
     (((x) & 0x00ff0000) >>  8) | \
     (((x) & 0x0000ff00) <<  8) | \
     (((x) & 0x000000ff) << 24)))

#define    bswap16(x) \
    ((uint16_t)              \
    ((((x) & 0xff00) >> 8) | \
     (((x) & 0x00ff) << 8)))


// Network order macros. Always use the gcc pre-defined macros!
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define ntohll(x)  ((uint64_t)(x))
#define htonll(x)  ((uint64_t)(x))
#define ntohl(x)   ((uint32_t)(x))
#define ntohs(x)   ((uint16_t)(x))
#define htonl(x)   ((uint32_t)(x))
#define htons(x)   ((uint16_t)(x))

#define htobe64(x) ((uint64_t)(x))
#define htobe32(x) ((uint32_t)(x))
#define htobe16(x) ((uint16_t)(x))
#define htole64(x) bswap64((uint64_t)(x))
#define htole32(x) bswap32((uint32_t)(x))
#define htole16(x) bswap16((uint16_t)(x))

#else /* #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ */
#define ntohll(x)  bswap64((uint64_t)(x))
#define htonll(x)  bswap64((uint64_t)(x))
#define ntohl(x)   bswap32((uint32_t)(x))
#define ntohs(x)   bswap16((uint16_t)(x))
#define htonl(x)   bswap32((uint32_t)(x))
#define htons(x)   bswap16((uint16_t)(x))
#define htobe64(x) bswap64((uint64_t)(x))
#define htobe32(x) bswap32((uint32_t)(x))
#define htobe16(x) bswap16((uint16_t)(x))
#define htole64(x) ((uint64_t)(x))
#define htole32(x) ((uint32_t)(x))
#define htole16(x) ((uint16_t)(x))

#endif

#endif  /* _INCLUDES_H_ */
