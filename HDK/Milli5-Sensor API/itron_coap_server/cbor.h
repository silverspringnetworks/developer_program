/*
 * Copyright SilverSpring Networks 2017.
 * All rights reserved.
 *
 * $Id: cbor.h 118327 2017-10-09 20:38:42Z araghunathan $
 */

#ifndef CBOR_CBOR_H_
#define CBOR_CBOR_H_

#define CBOR_TYPE_UINT          0x00
#define CBOR_TYPE_NINT          0x20
#define CBOR_TYPE_BYTES         0x40
#define CBOR_TYPE_TEXT          0x60
#define CBOR_TYPE_ARRAY         0x80
#define CBOR_TYPE_MAP           0xA0
#define CBOR_TYPE_TAG           0xC0
#define CBOR_TYPE_PRIMITIVE     0xE0

/* CBOR PRIMITIVE VALUES */
#define CBOR_PRIM_BOOL_FALSE          0x14
#define CBOR_PRIM_BOOL_TRUE           0x15
#define CBOR_PRIM_NULL                0x16
#define CBOR_PRIM_UNDEF               0x17
#define CBOR_PRIM_FLOAT32             0x1A
/* END CBOR PRIMITIVE VALUES */

#define CBOR_MJR0_THRESHOLD           0x17
#define CBOR_ADDL_BYTE_UINT8          0x18
#define CBOR_ADDL_BYTE_UINT16         0x19
#define CBOR_ADDL_BYTE_UINT32         0x1A
#define CBOR_ADDL_BYTE_UINT64         0x1B

#define CBOR_INDEF_LEN                0x1F
#define CBOR_INDEF_TERM               0xFF

#define CBOR_OK                    0
#define CBOR_ERR                  -1
#define CBOR_NO_MEM               (1 << 0)
#define CBOR_OUT_OF_DATA          (1 << 1)
#define CBOR_INVALID_TYPE         (1 << 2)
#define CBOR_INV_ADDNL_INFO       (1 << 3)
#define CBOR_NOT_WELL_FRMD        (1 << 4)

#define CBOR_DBG                  0
#if CBOR_DBG
#define CBOR_DBG_PRINT(...) printf(__VA_ARGS__)
#else
#define CBOR_DBG_PRINT(...)
#endif

struct cbor_buf {
    uint8_t *head;
    uint8_t *tail;
    uint8_t *next;
    int err;
};

uint32_t cbor_buf_get_len(struct cbor_buf *cbuf);

int cbor_enc_msg(struct cbor_buf *cbuf, char *msg);

/* CBOR Encoder APIs */
void cbor_enc_init(struct cbor_buf *cbuf, void *buf, int len);
int cbor_enc_int(struct cbor_buf *cbuf, int val);
int cbor_enc_text(struct cbor_buf *cbuf, const char *inp_string, int len);
int cbor_enc_bytes(struct cbor_buf *cbuf, const uint8_t *inp_bytes, int len);
int cbor_enc_array(struct cbor_buf *cbuf, int num_elements);
int cbor_enc_map(struct cbor_buf *cbuf, int num_elements);
void cbor_enc_indef_array(struct cbor_buf *cbuf);
void cbor_enc_indef_map(struct cbor_buf *cbuf);
void cbor_enc_indef_term(struct cbor_buf *cbuf);
int cbor_enc_prim_float32(struct cbor_buf *cbuf, float val);
void cbor_enc_prim_undef(struct cbor_buf *cbuf);
int cbor_enc_uint(struct cbor_buf *cbuf, uint32_t val);

/* CBOR Decoder API */
void cbor_dec_init(struct cbor_buf *cbuf, void *buf, int len);
uint8_t cbor_dec_major_type(struct cbor_buf *cbuf);
int cbor_dec_int(struct cbor_buf *cbuf, int *val);
const char * cbor_dec_text(struct cbor_buf *cbuf, int *str_len);
const uint8_t * cbor_dec_bytes(struct cbor_buf *cbuf, int *byte_len);
int cbor_dec_array(struct cbor_buf *cbuf);
int cbor_dec_map(struct cbor_buf *cbuf);
int cbor_dec_well_formed(struct cbor_buf *cbuf);
bool cbor_dec_indef_break(struct cbor_buf *cbuf);
int cbor_dec_primitive(struct cbor_buf *cbuf);
int cbor_dec_prim_float32(struct cbor_buf *cbuf, float *val);
int cbor_dec_uint(struct cbor_buf *cbuf, uint32_t *val);


#if CBOR_64_BIT /* include support for 64-bit integers on 32-bit CPU */
int cbor_enc_int64(struct cbor_buf *cbuf, int64_t val);
int cbor_dec_int64(struct cbor_buf *cbuf, int64_t *val);
#endif

#endif /* CBOR_CBOR_H_ */
