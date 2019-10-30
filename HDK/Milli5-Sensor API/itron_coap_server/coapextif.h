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



#ifndef INC_COAPEXTIF_H
#define INC_COAPEXTIF_H

#include "includes.h"
#include "errors.h"
#include "hbuf.h"
#include "exp_coap.h"

/* Resource data types, for TLV from the sensor, system data like time, etc. */
typedef enum packed_t {
    crdt_min_sens = 0,
    crdt_tilt = crdt_min_sens,
    crdt_shock,
    crdt_temp,
    crdt_humid,
    crdt_all,
    crdt_max_sens = crdt_all,
    crdt_min_log,
    crdt_log = crdt_min_log,
    crdt_max_log = crdt_log,
    crdt_min_sys,
    crdt_time_abs = crdt_min_sys,
    crdt_time_delta,
    crdt_stat_coap,
    crdt_stat_mnic_pwr,
    crdt_stat_lnic_ap_pwr,
    crdt_batt_cfg,
    crdt_sysvar,
    crdt_max_sys = crdt_sysvar,
    crdt_upg_img_ver_sys,
	crdt_upg_img_info_sys,
	crdt_upg_state_sys,
    crdt_none,                  /* no resource */
    crdt_max = crdt_none
} coap_res_data_type_t;

typedef struct packed_t {
    uint32_t time;
    uint32_t seq;
    uint16_t length;
    uint16_t pad;
    uint8_t payload[];
} coap_sens_data_t;


/* 
 * Configuration data from the backend. Attempting to be generic, and leave it
 * to the drivers to translate into what the sensor needs.
 */
typedef enum packed_t {
    csct_tilt = 0,
    csct_shock,
    csct_temp,
    csct_humid,
    csct_glbl,  /* global settings */
    csct_max    /* Must be last */
} coap_sens_cfg_type_t;
STATIC_ASSERT(sizeof(coap_sens_cfg_type_t) == sizeof(uint8_t));

/* Type and length */
typedef struct packed_t {
    union {                 /* Sensor type identifier. */
        coap_sens_cfg_type_t sct;
        coap_res_data_type_t rdt;
    } u;
    uint8_t l;              /* Length of this config data. */
} coap_sens_tl_t;

typedef struct packed_t coap_sens_cfg_data {
    coap_sens_tl_t tl;  /* Type and length */
    uint32_t poll_offs; /* sensor poll offset from midnight to start. */
    uint32_t poll_prd;  /* sensor poll period, how often to read it. */
    uint32_t obs_offs;  /* sensor obs offset from midnight to start. */
    uint32_t obs_prd;   /* Observe period, how often to send. */
    uint16_t obs_retry_prd; /* Period between retries when no ACK rxed. */
    uint8_t obs_retry_cnt;  /* Max number of retries. */
} coap_sens_cfg_data_t;

typedef struct packed_t coap_tilt_cfg_data {
    coap_sens_tl_t tl;      /* Type and length */
    uint32_t tilt;          /* Alert if |tilt| > tan(deg)^2 * 10000. */
    uint32_t tilt_hyst;     /* Tilt hysteresis, tan(deg)^2 * 10000. */
    uint8_t v_axis;         /* Vertical axis. */
    uint8_t enable;         /* Set/get enable sensor. */
} coap_tilt_cfg_data_t;

typedef struct packed_t coap_shock_cfg_data {
    coap_sens_tl_t tl;      /* Type and length */
    uint8_t shock_g;        /* Alert if |shock G| > shock_g. */
    uint16_t bandwidth;     /* bandwidth of shock sensor, hertz */
    uint8_t enable;         /* Set/get enable sensor. */
} coap_shock_cfg_data_t;

typedef struct packed_t coap_temp_cfg_data {
    coap_sens_tl_t tl;      /* Type and length */
    int8_t temp_min;        /* Alert if temp C < temp_min. */
    int8_t temp_max;        /* Alert if temp C > temp_max. */
    uint8_t temp_hyst;      /* Hysteresis. */
    uint8_t enable;         /* Set/get enable sensor. */
} coap_temp_cfg_data_t;

typedef struct packed_t coap_humid_cfg_data {
    coap_sens_tl_t tl;      /* Type and length */
    uint8_t relh_min;       /* Alert if relative humidity < relh_min. */
    uint8_t relh_max;       /* Alert if relative humidity > relh_max. */
    uint8_t relh_hyst;      /* Hysteresis. */
    uint8_t enable;         /* Set/get enable sensor. */
} coap_humid_cfg_data_t;
    

/* ACK callback */
typedef error_t (*coap_ack_cb_t)(void *cbctx, struct mbuf *m);

typedef struct {
    coap_ack_cb_t cb;       /* callback function */
    void *cbctx;            /* callback param */
} coap_ack_cb_info_t;

/* Observe callback. */
typedef error_t (*coap_obs_cb_t)(coap_res_data_type_t sdt, void *sh, 
                                 struct mbuf *m, uint8_t last, uint32_t dt,
                                 coap_ack_cb_info_t *cbi);

/* Specified for a GET /logistics/sens?id=all to get records from the flash. */
typedef struct packed_t {
    coap_sens_tl_t tl;
    uint32_t seq_num;   /* Read from this one, if possible. */
} coap_get_seq_num_t;

/* In response to a GET /system/sysvar?id=<id> to get arbitrary sysvars. */
typedef struct packed_t {
    coap_sens_tl_t tl;
    void *data[];
} coap_sysvar_data_t;

/* System data types */
typedef struct packed_t {
    coap_sens_tl_t tl;      /* type and length */
    uint32_t sec;           /* seconds */
    uint32_t msec;          /* milliseconds */
} coap_sys_time_data_t;


/* System stats types */
/* CoAP stats */
typedef struct {
    coap_sens_tl_t tl;      /* type and length */
    char pad[2];            /* align */
    struct coap_stats cs;   /* CoAP stats */
} coap_sys_coap_stats_t;

#define MAX_DEVID_LEN	10

typedef struct {
    uint32_t s;                 /* seconds */
    uint32_t ms;                /* milliseconds */
} coap_pwr_time_t;



typedef struct  {
    coap_sens_tl_t tl;          /* type and length, not including pad */
    char pad[2];                /* align */
	char cfg[];                 /* oqaque config data */
} coap_sys_batt_info_t;

typedef enum packed_t {
    cust_idle = 0,
    cust_init_passed = 1,
    cust_init_failed = 2,
    cust_txr_passed = 3,
    cust_txr_failed = 4,
    cust_verify_passed = 5,
    cust_verify_failed = 6,
    cust_activate_passed = 7,
    cust_activate_failed = 8,
} coap_upgrade_state_t;

typedef struct {
    uint8_t major;
    uint8_t minor;
    uint16_t revision;
} coap_upg_ver_t;

typedef struct {
    coap_sens_tl_t tl;      /* type and length */
    char pad[2];                /* align */
    coap_upg_ver_t ver;
} coap_sys_upg_ver_t ;

typedef struct {
    coap_sens_tl_t tl;      /* type and length */
    char pad[2];                /* align */
    coap_upg_ver_t ver;
    uint16_t flags;
    uint16_t img_crc;
    uint32_t img_len;
    uint32_t device_type;
} coap_sys_upg_img_info_t;

typedef struct {
    coap_sens_tl_t tl;      /* type and length */
    char pad[2];                /* align */
    coap_upg_ver_t upg_ver;
    uint32_t expected_offset;
    uint32_t img_len;
    uint16_t block_size;
    coap_upgrade_state_t state;
} coap_sys_upg_state_t ;

#endif /* INC_COAPEXTIF_H */
