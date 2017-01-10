#ifndef __LIB_READER_UPDATE_H__
#define __LIB_READER_UPDATE_H__


/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/


#define LIB_UPDATE_OK			1	//升级成功
#define LIB_UPDATE_ERR   		2	//升级失败

enum UPDATE_STATUS {
	READER_UPDATE_ING = 1,	//正在升级
	READER_UPDATE_SUCCESS,	//升级成功
	READER_UPDATE_FAILED,	//升级失败
};


/*获取软件版本号*/
char *lib_update_version(void);
/*升级接口*/
int lnt_reader_update(int fd, char *firmware_path);

/*获取读卡器升级状态*/
enum UPDATE_STATUS lib_get_update_status(void);


/*@*/
#ifdef __cplusplus
}
#endif
#endif

