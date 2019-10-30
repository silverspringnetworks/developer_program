/*
 * Copyright SilverSpring Networks 2017.
 * All rights reserved.
 *
 * $Id: cbor_encode.c 118327 2017-10-09 20:38:42Z araghunathan $
 */
#include <arduino.h>
#include <includes.h>

#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
//#include <arpa/inet.h>
#include "cbor.h"

#if CBOR_64_BIT
#error "64-bit integer support is not available"
#endif

#define SIZE_UINT8   sizeof(uint8_t)
#define SIZE_UINT16  sizeof(uint16_t)
#define SIZE_UINT    sizeof(uint32_t)
#define SIZE_UINT64  sizeof(uint64_t)

/*
 * function: cbor_buf_int
 * description: initialize a CBOR-specific struct used to serialize enoded
 * data.
 * input: cbuf -> ptr to cbor buf to be used during the encode session
 *        buf -> buffer to store encoded data.
 *        len -> size of buffer passed in.
 *        decode -> use this cbor buf for decode.
 */
void
cbor_enc_init(struct cbor_buf *cbuf, void *buf, int len)
{
    memset(cbuf, 0, sizeof(struct cbor_buf));
    cbuf->head = (uint8_t*)buf;
    cbuf->next = (uint8_t*)buf;
    cbuf->tail = cbuf->head + len;
    return;
}

/*
 * function: cbor_buf_get_len
 * description: Return the length of the data in the buffer.
 * input: cbuf -> ptr to cbor buf to be used during the encode session
 * output: Buffer data length.
 */
uint32_t
cbor_buf_get_len(struct cbor_buf *cbuf)
{
    return cbuf->next - cbuf->head;
}

/*
 * function:cbor_get_addnl_info
 * description: function that returns data for the additional
 * information field
 * input: val -> value to get the additional information for
 *        addnl_info -> ptr to save the CBOR mask used to represent length
 *        of val.
 * output: number of bytes corresponding to CBOR mask above.
 */
static int
cbor_get_addnl_info(uint32_t val, uint8_t *addnl_info)
{
    int retbytes;

    if (val <= CBOR_MJR0_THRESHOLD) {
        *addnl_info = val;
        retbytes = 0;
    } else if (val > CBOR_MJR0_THRESHOLD && val <= UINT8_MAX) {
        *addnl_info = CBOR_ADDL_BYTE_UINT8;
        retbytes = SIZE_UINT8;
    } else if (val > UINT8_MAX && val <= UINT16_MAX) {
        *addnl_info = CBOR_ADDL_BYTE_UINT16;
        retbytes = SIZE_UINT16;
    } else if (val > UINT16_MAX && val <= UINT32_MAX) {
        *addnl_info = CBOR_ADDL_BYTE_UINT32;
        retbytes = SIZE_UINT;
    }
#if CBOR_64_BIT            
    else if (val > UINT32_MAX && val <= UINT64_MAX) {
        *addnl_info = CBOR_ADDL_BYTE_UINT64;
        retbytes = SIZE_UINT64;
    }
#endif    
    return retbytes;
}

/* Variable length encoder for integer types - UINT/NINT and ARRAY/MAP dimensions */
static int
cbor_enc_length(uint8_t major_type, struct cbor_buf * cbuf, uint32_t val)
{
    int addnlBytes;
    uint8_t additionalInfo;

    addnlBytes = cbor_get_addnl_info(val, &additionalInfo);

    if ((cbuf->next + addnlBytes) > cbuf->tail) {
        cbuf->err = CBOR_NO_MEM;
        return CBOR_ERR;
    }

    *cbuf->next++ = major_type | additionalInfo;

    if (addnlBytes) {
        switch(addnlBytes) {
        case SIZE_UINT8:
            break;
        case SIZE_UINT16:
            val = htons(val);
            break;
        case SIZE_UINT:
            val = htonl(val);
            break;
#if CBOR_64_BIT
        case SIZE_UINT64:
            /* htonll breaks regress builds on Jenkins. COmmenting this out
             * for now until we do the byte swap.
             */
            break;
#endif
        default:
            /* TODO - error handling */
           cbuf->err = CBOR_INV_ADDNL_INFO;
           return CBOR_ERR;
        }
        memcpy(cbuf->next, &val, addnlBytes);
        cbuf->next += addnlBytes;
    }
    return CBOR_OK;
}

/*
 * description: cbor_enc_int
 * input: val: Value to be encoded
 *        cbuf: ptr to CBOR stream buffer pointing to the location where
 *        encoded bytes are to be saved.
 * output: Length of encoded bytes (positive values)
 *         Error (negative values)
 */
int
cbor_enc_int(struct cbor_buf *cbuf, int val)
{
    uint8_t major_type = CBOR_TYPE_UINT;

    if (val < 0) {
        val = -1 - val;
        major_type = CBOR_TYPE_NINT;
    }

    return (cbor_enc_length(major_type, cbuf, val));
}

int
cbor_enc_uint(struct cbor_buf *cbuf, uint32_t val)
{
    uint8_t major_type = CBOR_TYPE_UINT;

    return (cbor_enc_length(major_type, cbuf, val));
}

#if CBOR_64_BIT
/*
 * function: cbor_enc_int64
 * description: same as cbor_enc_int but encoded 64 bit values and uint32
 * values greater than 2,147,483,647.
 */
int
cbor_enc_int64(struct cbor_buf *cbuf, int64_t val)
{
    uint8_t major_type = CBOR_TYPE_UINT;

    if (val < 0) {
        val = -1 - val;
        major_type = CBOR_TYPE_NINT;
    }

    return (cbor_enc_length(major_type, cbuf, val));
}
#endif

/*
 * function: cbor_enc_text
 * description: CBOR encode a text string.
 * input: cbuf -> ptr to CBOR stream buffer pointing to the location where
 *        encoded bytes are to be saved.
 *        inp_string -> ptr to string to be encoded.
 *        len -> length of string/char to be encoded.
 * output: status of encode operation.
 */
int
cbor_enc_text(struct cbor_buf *cbuf,
                const char *inp_string, int len)
{
    int rc = CBOR_OK;
    rc = cbor_enc_length(CBOR_TYPE_TEXT, cbuf, len);

    if (!rc) {
        if (len > (cbuf->tail - cbuf->next)) {
            cbuf->err = CBOR_NO_MEM;
            rc = CBOR_ERR;
        } else {
            /* Now, copy the string */
            memcpy(cbuf->next, inp_string, len);
            cbuf->next += len;
        }
    }
    return rc;
}

/*
 * function: cbor_enc_bytes
 * description: CBOR encode a byte string.
 * input: cbuf -> ptr to CBOR stream buffer pointing to the location where
 *        encoded bytes are to be saved.
 *        inp_bytes -> ptr to bytes to be encoded.
 *        len -> length of string/char to be encoded.
 * output: status of encode operation.
 */
int
cbor_enc_bytes(struct cbor_buf *cbuf,
                const uint8_t *inp_bytes, int len)
{
    int rc = CBOR_OK;

    rc = cbor_enc_length(CBOR_TYPE_BYTES, cbuf, len);

    if (!rc) {
        if (len > (cbuf->tail - cbuf->next)) {
            cbuf->err = CBOR_NO_MEM;
            rc = CBOR_ERR;
        } else {
            /* Now, copy the string */
            memcpy(cbuf->next, inp_bytes, len);
            cbuf->next += len;
        }
    }
    return rc;
}

/*
 * function: cbor_enc_array
 * description: CBOR encode an array.
 * input: cbuf -> ptr to CBOR stream buffer pointing to the location where
 *        encoded bytes are to be saved.
 *        num_elements -> number of elements in the array
 * output: status of encode operation.
 */
int
cbor_enc_array(struct cbor_buf *cbuf, int num_elements)
{
    return(cbor_enc_length(CBOR_TYPE_ARRAY, cbuf, num_elements));
}
/*
 * function: cbor_enc_map
 * description: CBOR encode a map.
 * input: cbuf -> ptr to CBOR stream buffer pointing to the location where
 *        encoded bytes are to be saved.
 *        num_elements -> number of elements in the map
 * output: status of encode operation.
 */
int
cbor_enc_map(struct cbor_buf *cbuf, int num_elements)
{
    return(cbor_enc_length(CBOR_TYPE_MAP, cbuf, num_elements));
}

void
cbor_enc_indef_array(struct cbor_buf *cbuf)
{
    /* Encoding indefinite array requires atleast one more byte
     * for terminator;
     */
    if ((cbuf->tail - cbuf->next) < 2) {
        cbuf->err = CBOR_NO_MEM;
        return;
    }
    *cbuf->next++ = CBOR_TYPE_ARRAY | CBOR_INDEF_LEN;
}

void
cbor_enc_indef_term(struct cbor_buf *cbuf)
{
    if ((cbuf->tail - cbuf->next) < 1) {
        cbuf->err = CBOR_NO_MEM;
        return;
    }
    *cbuf->next++ = CBOR_INDEF_TERM;
}

void
cbor_enc_indef_map(struct cbor_buf *cbuf)
{
    if ((cbuf->tail - cbuf->next) < 2) {
        cbuf->err = CBOR_NO_MEM;
        return;
    }
    *cbuf->next++ = CBOR_TYPE_MAP | CBOR_INDEF_LEN;
}

void
cbor_enc_prim_null(struct cbor_buf *cbuf)
{
    if ((cbuf->tail - cbuf->next) < 1) {
        cbuf->err = CBOR_NO_MEM;
        return;
    }
    *cbuf->next++ = CBOR_TYPE_PRIMITIVE | CBOR_PRIM_NULL;
}

int
cbor_enc_prim_bool(struct cbor_buf *cbuf, bool val)
{
    if ((cbuf->tail - cbuf->next) < 1) {
        cbuf->err = CBOR_NO_MEM;
        return CBOR_ERR;
    }
    if (val) {
        *cbuf->next++ = CBOR_TYPE_PRIMITIVE | CBOR_PRIM_BOOL_TRUE;
    } else {
        *cbuf->next++ = CBOR_TYPE_PRIMITIVE | CBOR_PRIM_BOOL_FALSE;
    }
    return CBOR_OK;
}

void
cbor_enc_prim_undef(struct cbor_buf *cbuf) {
    if ((cbuf->tail - cbuf->next) < 1) {
        cbuf->err = CBOR_NO_MEM;
    }
    *cbuf->next++ = CBOR_TYPE_PRIMITIVE | CBOR_PRIM_UNDEF;
}

int
cbor_enc_prim_float32(struct cbor_buf *cbuf, float val)
{
    typedef union {
        uint32_t u32;
        float    f;
    }fl_u32;

    fl_u32 temp;
    temp.f = val;
    temp.u32 = htonl(temp.u32);


    if ((cbuf->tail - cbuf->next) < sizeof(float)) {
        cbuf->err = CBOR_NO_MEM;
        return CBOR_ERR;
    }
    *cbuf->next++ = CBOR_TYPE_PRIMITIVE | CBOR_PRIM_FLOAT32;
    memcpy(cbuf->next, &temp.u32, sizeof(float));
    cbuf->next += sizeof(float);
    return CBOR_OK;
}
