#ifndef __LIB_4G_H__
#define __LIB_4G_H__

/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>


struct lib_4g_gga
{
	double latitude;  				//纬度，格式为 ddmm.mmmmm（度分格式）
	unsigned char ns_indicator;  		//纬度半球，N 或 S（北纬或南纬）
	double longitude; 				//经度，格式为 dddmm.mmmmm（度分格式）
	unsigned char ew_indicator; 		//经度半球，E 或 W（东经或西经）
	int status; 					// GPS 状态，0=未定位，1=定位 
	int satellite; 					//正在使用的用于定位的卫星数量（00~12） 
};
typedef struct lib_4g_gga lib_4g_gga_t;


struct gps_attr
{
	unsigned char gga:1;
	unsigned char tm:1;
	unsigned char rfu:6;
}__attribute__((packed));
typedef struct gps_attr gps_attr_t;

enum ENUM_4G_CONNECT_STATUS
{
	ECM_4G_UNCONNECT = 0,  // 未定位
	ECM_4G_CONNECT,	    // 非差分定位
	ECM_4G_SERIAL_ERROR,			// 差分定位
};

enum CARD_TYPE
{
	CARD_CMCC = 1,  // 中国移动
	CARD_CUCC,	    // 中国联通
	CARD_CTCC,		// 中国电信
};

enum CARD_STATUS
{
	CARD_STATUS_READY = 1,  // 准备好了
	CARD_STATUS_SIM_PIN,	    // 等待加密，卡是加密了
	CARD_STATUS_NO_INSERTED,		// 没卡
	CARD_STATUS_NO_INSERTED_AND_ERROR,		// 没卡和Numerical Extended error result code is enabled.
};


typedef enum __bool { false = 0, true = 1, } bool;

int lib_4g_get_csq(int fd);
int lib_4g_ecm_disconnect(int fd);
int lib_4g_ecm_connect(int fd);
int lib_4g_ecm_connect_status(int fd);



int lib_4g_ate0(int fd);
int lib_4g_decode(int fd, char* key);			// 解密4G卡，否者某些步骤失败
int lib_4g_restart(int fd);
int lib_4g_setECMforWindows_and_restart(int fd);
int lib_4g_setECMforLinux_and_restart(int fd);
int lib_4g_get_basic_info(int fd);
int lib_4g_CUCC_CTCC_dialing(int fd);
int lib_4g_CMCC_dialing(int fd);
int lib_4g_getGPS_init(int fd);
int lib_4g_get_gps_info(int fd, char *buf, int msec);
int lib_4g_analytic_zgpsr(char *rxbuf, struct tm *my_4g_tm, lib_4g_gga_t *my_4g_gga, gps_attr_t *attr);
int lib_4g_check_cpin(int fd);
int lib_4g_check_cops(int fd);
int lib_4g_zsnt(int fd);









int get_yday(int year,int mon,int day);
int leap(int y);
int monthday(int m, int team);
void gps_show_tm(struct tm *tm);
void gps_show_gga(struct lib_4g_gga *gga);
void printf_warning(char *buf);






/*@*/
#ifdef __cplusplus
}
#endif
#endif


