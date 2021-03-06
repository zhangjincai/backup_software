#ifndef __LIB_SHA204_H__
#define __LIB_SHA204_H__


/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/


#define LIB_WAKE_UP_SUCCESS 0

#define LIB_EV_OK			1	//验证通过
#define LIB_EV_ERR   		2	//验证失败


/*获取软件版本号*/
char *LIB_EV_Version(void);
/*验证接口*/
int LIB_Encry_Verify(void);
/*读取节点机设备信息*/
int read_device_info(unsigned char *device_info);


/*@*/
#ifdef __cplusplus
}
#endif
#endif

