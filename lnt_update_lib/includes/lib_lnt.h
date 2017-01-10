#ifndef __LIB_LNT_H__
#define __LIB_LNT_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

#include "lnt.h"

#define LIB_LNT_NULL				LNT_NULL 
#define LIB_LNT_EOK					LNT_EOK	
#define LIB_LNT_ERROR				LNT_ERROR
#define LIB_LNT_ETIMEOUT			LNT_ETIMEOUT
#define LIB_LNT_EFULL				LNT_EFULL
#define LIB_LNT_EEMPTY				LNT_EEMPTY
#define LIB_LNT_ENOMEM 				LNT_ENOMEM 
#define LIB_LNT_EXCMEM				LNT_EXCMEM
#define LIB_LNT_EBUSY				LNT_EBUSY
#define LIB_LNT_NOT_SUPPORT			LNT_NOT_SUPPORT	

#define LIB_LNT_TRUE				LNT_TRUE
#define LIB_LNT_FALSE				LNT_FALSE



typedef lnt_getversion_ack_t lib_lnt_getversion_ack_t;
typedef lnt_core_init_req_t lib_lnt_core_init_req_t;
typedef lnt_core_init_ack_t lib_lnt_core_init_ack_t;
typedef lnt_parameter_set_req_t lib_lnt_parameter_set_req_t;
typedef lnt_qry_ticket_info_ack_t lib_lnt_qry_ticket_info_ack_t;
typedef rent_info_t lib_rent_info_t;
typedef lnt_rent_info_req_t lib_lnt_rent_info_req_t;
typedef lnt_rent_info_ack_t lib_lnt_rent_info_ack_t;
typedef lnt_rent_info_t lib_lnt_rent_info_t;
typedef lnt_blacklist_record_ack_t lib_lnt_blacklist_record_ack_t;
typedef lnt_trade_record_ack_t lib_lnt_trade_record_ack_t;
typedef lnt_history_record_ack_t lib_lnt_history_record_ack_t;
typedef lnt_consume_req_t lib_lnt_consume_req_t;
	
typedef lnt_r_init_ack_t lib_lnt_r_init_ack_t;

 
typedef struct lnt_config
{
	char tty[32];
	unsigned int baudrate;
}lib_lnt_config_t;


int lib_lnt_init(lib_lnt_config_t *config);
int lib_lnt_release(void);

int lib_lnt_get_firmware_version(char *version);
void lib_lnt_utils_time_bcd_yymmddhhMMss(unsigned char tm[6]);
void lib_lnt_utils_time_bcd_yymmddhhMM(unsigned char tm[5]);
void lib_lnt_utils_time_bcd_yymmdd(unsigned char tm[3]);


int lib_lnt_get_version(lib_lnt_getversion_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_core_init(lib_lnt_core_init_req_t *req, lib_lnt_core_init_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_set_parameter(lib_lnt_parameter_set_req_t *req, unsigned char *stat, const unsigned int msec);
int lib_lnt_qry_ticket_info(lib_lnt_qry_ticket_info_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_qry_rent_info(lib_lnt_rent_info_req_t *req, lib_lnt_rent_info_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_set_rent_info(lib_lnt_rent_info_t *rent, unsigned char *stat, const unsigned int msec);
int lib_lnt_get_history_record(lib_lnt_history_record_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_set_blacklist_flag(lib_lnt_blacklist_record_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_get_trade_record(lib_lnt_trade_record_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_consume(lib_lnt_consume_req_t *req, unsigned char *stat, const unsigned int msec);

int lib_lnt_CLA_INS_pkg(void *s_ptr, const unsigned int s_len, void *d_ptr, const unsigned int d_len, unsigned char *d_stat, const unsigned int msec);

int lib_lnt_r_init(lnt_r_init_ack_t *ack, unsigned char *stat, const unsigned int msec); 
int lib_lnt_r_ac_login_1(lnt_r_ac_login_1_ack_t *ack, unsigned char *stat, const unsigned int msec);


int lib_lnt_r_deposit_init(lnt_r_deposit_init_req_t *req, lnt_r_deposit_init_ack_t *ack, unsigned char *stat, const unsigned int msec);
int lib_lnt_r_deposit_process(lnt_r_deposit_process_req_t *req, lnt_r_deposit_process_ack_t *ack, unsigned char *stat, const unsigned int msec); 

 
/*@*/
#ifdef __cplusplus
}
#endif
#endif

