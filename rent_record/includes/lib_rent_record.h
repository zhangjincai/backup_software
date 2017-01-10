#ifndef __LIB_RENT_RECORD_H__
#define __LIB_RENT_RECORD_H__


/*@*/
#ifdef __cplusplus
extern "C" {
#endif
/*@*/


struct rent_info_fmt		//租用记录格式
{ 
	unsigned char bike_NO[32];		//自行车编号
	unsigned char rent_name[64];	//租车点名称
	unsigned char rent_time[32];	//租车时间
	unsigned char return_name[64];	//还车点名称
	unsigned char return_time[32];	//还车时间
	unsigned char used_time[16];	//骑行时间
	unsigned char fee[16];			//扣费
}__attribute__((packed));
typedef struct rent_info_fmt rent_info_fmt_t;



int ParseRentBikeRecord(char* buff,int len);




/*@*/
#ifdef __cplusplus
}
#endif

#endif/*#ifndef __LIB_RENT_RECORD_H__*/

