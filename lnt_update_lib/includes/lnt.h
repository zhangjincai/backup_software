#ifndef __LNT_H__
#define __LNT_H__

#include <arpa/inet.h>

#define LNT_NULL				(NULL) 
#define LNT_EOK					(0)  //正常
#define LNT_ERROR				(-1) //错误
#define LNT_ETIMEOUT			(-2) //超时
#define LNT_EFULL				(-3) //满
#define LNT_EEMPTY				(-4) //空
#define LNT_ENOMEM 				(-5) //内存不够
#define LNT_EXCMEM				(-6) //内存越界
#define LNT_EBUSY				(-7) //忙
#define LNT_NOT_SUPPORT			(-8) //不支持该指令

#define LNT_TRUE				(1)
#define LNT_FALSE				(0)

#define LNT_CHAR_MAX			0xff
#define LNT_SHORT_MAX			0xffff
#define LNT_INT_MAX				0xffffffff

#define LNT_NTOHS(n)				ntohs(n)	
#define LNT_HTONS(n)				htons(n)
#define LNT_NTOHL(n)				ntohl(n)
#define LNT_HTONL(n)				htonl(n)



/* 日志定义 */
//#define LNT_LOG_RUN

#ifdef LNT_LOG_RUN
#include <syslog.h>
#define SYS_LOG_EMERG(fmt, args...) 		syslog(LOG_EMERG, fmt, ##args)
#define SYS_LOG_ALERT(fmt, args...) 			syslog(LOG_ALERT, fmt, ##args)
#define SYS_LOG_CRIT(fmt, args...) 			syslog(LOG_CRIT, fmt, ##args)
#define SYS_LOG_ERR(fmt, args...) 			syslog(LOG_ERR, fmt, ##args)
#define SYS_LOG_WARNING(fmt, args...) 		syslog(LOG_WARNING, fmt, ##args) 
#define SYS_LOG_NOTICE(fmt, args...)  		syslog(LOG_NOTICE, fmt, ##args)
#define SYS_LOG_INFO(fmt, args...) 			syslog(LOG_INFO, fmt, ##args)
#define SYS_LOG_DEBUG(fmt, args...) 		syslog(LOG_DEBUG, fmt, ##args)
#else
#define SYS_LOG_EMERG(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_ALERT(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_CRIT(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_ERR(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_WARNING(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_NOTICE(fmt, args...) 		fprintf(stderr, fmt, ##args)
#define SYS_LOG_INFO(fmt, args...) 			fprintf(stderr, fmt, ##args)
#define SYS_LOG_DEBUG(fmt, args...) 		fprintf(stderr, fmt, ##args)
#endif


/*
* 岭南通模块错误码
*/
#define LNT_MOC_ERROR_NORMAL					0x00  //操作正常
#define LNT_MOC_ERROR_SAM_EMPTY				0x60 //没有安装SAM卡
#define LNT_MOC_ERROR_INIT						0x61 //SAM卡初始化错误或未初始化
#define LNT_MOC_ERROR_PIN						0x62 //SAM卡检验PIN错误
#define LNT_MOC_ERROR_READ						0x65 //SAM卡读错误
#define LNT_MOC_ERROR_WRITE					0x66 //SAM卡写错误
#define LNT_MOC_ERROR_CARD_NOTAG				0x80  //没有卡
#define LNT_MOC_ERROR_CARD_AUTH				0x83  //认证卡片错误
#define LNT_MOC_ERROR_CARD_OP_FAIL			0x89  //卡片操作失败
#define LNT_MOC_ERROR_CARD_UNKOWN			0x90  //非岭南通卡
#define LNT_MOC_ERROR_CARD_OUTOFDATE			0x91  //卡片超出有效期
#define LNT_MOC_ERROR_CARD_NAMELIST			0x94   //黑名单
#define LNT_MOC_ERROR_CARD_OWEPTY			0x95   //钱包余额不
#define LNT_MOC_ERROR_CARD_OVERLIMIT			0x96   //钱包余额超出上限
#define LNT_MOC_ERROR_CARD_PURSE_UNUSED		0x97   //钱包未启用
#define LNT_MOC_ERROR_TS_PENDING				0xB0   //交易操作中途中断
#define LNT_MOC_ERROR_LASTTIME_TRADE_FAIL		0xD4   //上次交易未成功
#define LNT_MOC_ERROR_INVALID_PARAM			0xE3	   //参数错误


#define LNT_MOC_ERROR_LILIN_NOT_CPU_CARD		0xFE	   //非CPU卡

/*
 * 设备响应状态码
 */
#define LNT_SW_ERROR_NORMAL					0x9000 //成功执行,无错误
#define LNT_SW_ERROR_LENGTH					0x6700 //长度不对
#define LNT_SW_ERROR_INS						0x6D00 //INS错误
#define LNT_SW_ERROR_CLA						0x6E00 //CLA错误
#define LNT_SW_ERROR_DATA						0x6F00 //数据无效
#define LNT_SW_ERROR_AUTH						0x6300 //认证失败


/******************************************************************
*						岭南通读卡器固件升级
********************************************************************/

/*
* 升级状态设置
*/
struct update_stat_set{
	unsigned char mode;//模式
	unsigned short total_packets;//总数据包数,若"模式"为0x02则无此域
}__attribute__((packed));
typedef struct update_stat_set update_stat_set_t;

/*
* 预升级响应
*/
struct pre_update_ack{
	unsigned int next_position;
}__attribute__((packed));
typedef struct pre_update_ack pre_update_ack_t;

/*
* 升级响应
*/
struct update_ack{
	unsigned char retcode[5];
}__attribute__((packed));
typedef struct update_ack update_ack_t;







/******************************************************************
*						岭南通读卡器指令
********************************************************************/
/*
* 岭南通读卡器指令--广州公共自行车
*/
#define LNT_GETVERSION				0x60 	//获取程序版本
#define LNT_CORE_INIT				0xD2		//核心模块初始化
#define LNT_QRY_TICKET_INFO			0xD3        //查询卡信息
#define LNT_QRY_RENT_INFO			0xD9         //查询租还信息
#define LNT_SET_RENT_INFO			0xDA		//设置租还信息
#define LNT_CONSUME					0xD4         //消费
#define LNT_SET_BL_FLAG				0xD5         //黑名单设置
#define LNT_GET_HIS_RECORD			0xD6         //最近历史交易记录
#define LNT_SET_PARAM				0xD7         //参数设置
#define LNT_GET_TRANS				0xD8         //获取交易记录

#define LNT_DEPOSIT_INIT			0xE3         //押金初始化
#define LNT_DEPOSIT_PROCESS			0xE4        //押金处理


/*
 * 岭南通核心模块充值协议
 */
#define LNT_R_CMD					0xFF//0x40
#define LNT_R_CLA					0xBB
#define LNT_R_INIT_INS				0x10
#define LNT_R_AC_LOGIN_1_INS		0xC8




/*
 * 押金标志
 */
struct deposit_sign
{
	unsigned char rfu1:2;
	unsigned char type:2;
	unsigned char foshan:1;
	unsigned char guangzhou:1;
	unsigned char rfu2:2;
}__attribute__((packed));
typedef struct deposit_sign deposit_sign_t;


/*
 * 票卡信息数据说明
 */
struct ticket_info
{
	unsigned int blance;         	//余额
	unsigned char pid[8];			//物理卡号(纯M1卡前补4个0)
	unsigned char lid[8];           //逻辑卡号
	unsigned short ttype;			//卡片类型
	unsigned char ctim[4];			//充值时间(YYYYMMDD)
	unsigned char pursetype;		//钱包类型(0x01:M1钱包, 0x02:CPU钱包)
	unsigned char areaid;			//区域代码
	unsigned char areacardtype[2];	//区域卡代码
	unsigned char areacarddate[4];	//区域卡类型有效期
	unsigned char dflag;			//广州佛山押金标志
	unsigned char dflagother[4];	//广州以外的押金标志
	unsigned short pcode;			//运营商编码
	unsigned char pmflag;			//运营商会员标志
}__attribute__((packed));
typedef struct ticket_info ticket_info_t;

/*
 * 交易记录
 */
struct trade_record
{
	unsigned char rectype; 			//记录类型
	unsigned char areaid;			//卡片区域代码
	unsigned char ctype[2];			//卡类型
	unsigned char areacardtype[2];	//区域卡类型
	unsigned int opid:24;			//操作员编码,3字节
	unsigned char curtransdevid[6];	//本次交易设备编号
	unsigned int offlinetranssn; 		////脱机交易流水
	unsigned char curtransdtime[7];	//本次交易日期时间
	unsigned char lid[8];               		//逻辑卡号
	unsigned char pid[8];			//物理卡号(纯M1卡前补4个0)
	unsigned int amount;			//交易金额
	unsigned int priceofticket;		//票价
	unsigned int blance;         		//交易后卡内余额
	unsigned char ttype;	      		//交易类型
	unsigned char addittype;		//附加交易类型
	unsigned short rechargettimes;	//充值交易次数
	unsigned short consumettimes;      //消费交易次数
	unsigned char lasttdevid[6];		//上次交易设备编号
	unsigned char lasttdtime[7];		//上次交易日期时间
	unsigned int authcode;			//交易认证码
	unsigned char rfu[13];			//备用
}__attribute__((packed));
typedef struct trade_record trade_record_t;

/*
 * 黑名单记录
 */
struct blacklist_record
{
	unsigned char pid[8];			//物理卡号
	unsigned char lid[8];			//逻辑卡号
}__attribute__((packed));
typedef struct blacklist_record blacklist_record_t;

/*
 * M1历史记录
 */
struct m1_history_record
{
	unsigned short spid;          //服务商代码
	unsigned char ttype;	      //交易类型
	unsigned int amount:24;     //交易金额(高位在前)
	unsigned char samid[4]; 	      //SAM卡号
	unsigned char dtime[6];        //交易时间(YYMMDDHHMMSS)
}__attribute__((packed));
typedef struct m1_history_record m1_history_record_t; 

/*
 * CPU历史记录
 */
struct cpu_history_record
{
	unsigned short spid;          //交易序号(高位在前)
	unsigned int ovd:24; 	    //透支限额(高位在前
	unsigned int amount;    		//交易金额(高位在前)
	unsigned char ttype;	      //交易类型
	unsigned char tid[6];	      //终端机编号
	unsigned char date[4]; 	      //交易日期(XXMMDD)
	unsigned char time[3];        //交易时间(HHMMSS)
}__attribute__((packed));
typedef struct cpu_history_record cpu_history_record_t; 

/*
 * 租还车信息
 */
struct rent_info
{
	unsigned char app_locker;       //应用锁定标志
	unsigned short provider;		//服务营运代码
	unsigned char version_no:4;		//版本序号--低4位
	unsigned char rent_flag:4;		//租还车标志--高4位
	unsigned char member_level:4;	//会员级别--低4位
	unsigned char member_type:4;	//会员类型--高4位
	unsigned char ticket_type;		//票种标记
	unsigned short accum;			//次数/积分/天数
	unsigned short cur_accum;		//当次消费次数/积分/天数
	unsigned short cur_deal_sn;		//当次交易流水
	unsigned char start_time[3];		//启用时间   			YYMMDD
	unsigned char  cur_deal_date[5];	//当次交易日期时间   YYMMDDHHMM
	unsigned char site_no[6]; 		//站点编号
	unsigned char lock_no[4];		//锁桩编号
	unsigned char bike_no[4];		//自行车编号
	unsigned short accu_time_of_day; //当日累计时长
}__attribute__((packed));
typedef struct rent_info rent_info_t;

 

/*
* 获取程序版本
*/
struct lnt_getversion_ack
{
	unsigned char version[32]; 		//ASCII
}__attribute__((packed));
typedef struct lnt_getversion_ack lnt_getversion_ack_t;

/*
 * 核心模块初始化
 */
struct lnt_core_init_req
{
	unsigned char spno[2];//服务商代码
}__attribute__((packed));
typedef struct lnt_core_init_req lnt_core_init_req_t;

struct lnt_core_init_ack
{
	unsigned char psam[4];//PSAM卡号
}__attribute__((packed));
typedef struct lnt_core_init_ack lnt_core_init_ack_t;

/*
 * 查询票卡信息
 */
 struct lnt_qry_ticket_info_ack
{
	unsigned char length;
	struct ticket_info ticket;
}__attribute__((packed));
typedef  struct lnt_qry_ticket_info_ack lnt_qry_ticket_info_ack_t;

/*
 * 查询租还车信息
 */
struct lnt_rent_info_req
{
	unsigned char id;//复合应用类型标识符
}__attribute__((packed));
typedef struct lnt_rent_info_req lnt_rent_info_req_t;

struct lnt_rent_info_ack
{
	unsigned char length;
	struct rent_info rent;
}__attribute__((packed));
typedef struct lnt_rent_info_ack lnt_rent_info_ack_t;

struct lnt_rent_info
{
	unsigned char id;
	unsigned char length;
	struct rent_info rent;
}__attribute__((packed));
typedef struct lnt_rent_info lnt_rent_info_t;


/*
 * 消费
 */
#define LNT_ATY_RENT		0x11		//租车
#define LNT_ATY_RETURN		0x17		//还车
struct lnt_consume_req
{
	unsigned int amount;		//交易金额(低位在前)
	unsigned int fee;			//应收金额(低位在前)
	unsigned char time[6];		//YYMMDDHHMMSS
	unsigned char phyiscalid[8];	//物理卡号
	unsigned char aty;			//附加交易类型
}__attribute__((packed));
typedef struct lnt_consume_req lnt_consume_req_t;

/*
 * 参数设置
 */
struct lnt_parameter_set_req
{
	unsigned char conpa;//消费初始参数
	unsigned char cvad;//卡离线有效期启用
}__attribute__((packed));
typedef struct lnt_parameter_set_req lnt_parameter_set_req_t;

/*
 * 黑名单设置
 */
struct lnt_blacklist_record_ack
{
	struct blacklist_record blacklist;
}__attribute__((packed));
typedef struct lnt_blacklist_record_ack lnt_blacklist_record_ack_t;

/*
 * 最近历史交易记录
 */
struct lnt_history_record_ack
{
	unsigned char ticket_type;
	union{
		struct m1_history_record m1_his;
		struct cpu_history_record cpu_his;
	}s_un;
}__attribute__((packed));
typedef struct lnt_history_record_ack lnt_history_record_ack_t;


/*
 * 获取交易数据
 */
struct lnt_trade_record_ack
{
	struct trade_record record;
}__attribute__((packed));
typedef struct lnt_trade_record_ack lnt_trade_record_ack_t;



/*
 * *******************  岭南通核心模块充值协议 *************************
 */
/*
 * R_INIT(读卡初始化)
 */
struct lnt_r_init_ack
{
	unsigned char pki[4];   	 //PKI管理卡号
	unsigned char fw_ver[8];  //读卡器固件版本
	unsigned short sw;    	 //状态码
}__attribute__((packed));
typedef struct lnt_r_init_ack lnt_r_init_ack_t;

/*
 * R_AC_LOGIN_1(签到步骤1)
 */
struct lnt_r_ac_login_1_ack
{
	unsigned char login1_data[8+128+2];//+2
}__attribute__((packed));
typedef struct lnt_r_ac_login_1_ack lnt_r_ac_login_1_ack_t;

/*
 * R_AC_LOGIN_2(签到步骤2)
 */
struct lnt_r_ac_login_2_req
{
	unsigned char login2_data[8+128+5];//+5
}__attribute__((packed));
typedef struct lnt_r_ac_login_2_req lnt_r_ac_login_2_req_t;

struct lnt_r_ac_login_2_ack
{
	unsigned char rfu[46];
	unsigned char login1_data[8+128+2];//+2
}__attribute__((packed));
typedef struct lnt_r_ac_login_2_ack lnt_r_ac_login_2_ack_t;

/*
 * R_PUB_QRY_HIS(查询卡交易信息)
 */ 
struct card_status_16bytes 
{
	unsigned short spid;          //服务商代码
	unsigned char ttype;	      //交易类型			//0x17:普通消费,0x14:充值,0x13:地铁消费
	unsigned int amount:24;     //交易金额(高位在前)
	unsigned char samid[4]; 	      //SAM卡号
	unsigned char dtime[6];        //交易时间(YYMMDDHHMMSS)
}__attribute__((packed));

struct card_status_18bytes 
{
	unsigned int amount;    		//交易金额(高位在前)
	unsigned char ttype_ident;  //交易类型标识
	unsigned char tid[6];	      //终端机编号
	unsigned char rfu[2];	      //备用
	unsigned char date[4]; 	      //交易日期(XXMMDD)
	unsigned char ttype;	      //交易类型     //0x17:普通消费,0x14:充值,0x13:地铁消费
}__attribute__((packed));

struct lnt_r_pub_qry_his_ack
{
	unsigned char sak;
	unsigned char card_status;
	union{
		struct card_status_16bytes b16;
		struct card_status_18bytes b18;
	}s_un;
}__attribute__((packed));
typedef struct lnt_r_pub_qry_his_ack lnt_r_pub_qry_his_ack_t;

/*
 * R_PUB_QRY_PHYSICS_INFO(查询卡物理信息)
 */
struct lnt_r_pub_qry_physics_info_NoEncry_ack  //不加密
{
	unsigned char data[51];
	unsigned short sw;
}__attribute__((packed));
typedef struct lnt_r_pub_qry_physics_info_NoEncry_ack lnt_r_pub_qry_physics_info_NoEncry_ack_t;

struct lnt_r_pub_qry_physics_info_Encry_ack  //加密
{
	unsigned char data[80];
	unsigned short sw;
}__attribute__((packed));
typedef struct lnt_r_pub_qry_physics_info_Encry_ack lnt_r_pub_qry_physics_info_Encry_ack_t;

/*
 * R_PUB_SET_READCRADINFO(设置读卡器+卡信息)
 */
 struct lnt_r_pub_set_readcardinfo_req
{
	unsigned char data[8+72+5];
}__attribute__((packed));
typedef  struct lnt_r_pub_set_readcardinfo_req lnt_r_pub_set_readcardinfo_req_t;

 struct lnt_r_pub_set_readcardinfo_ack
{
	unsigned char data[72];   //各种类型的票卡返回不同
	unsigned short sw;
}__attribute__((packed));
typedef  struct lnt_r_pub_set_readcardinfo_ack lnt_r_pub_set_readcardinfo_ack_t;

///////////////////////////////////////////////////////
/*
 * R_CPU_LOAD_INIT(CPU钱包圈存初始化)
 */
struct lnt_r_cpu_load_init_req
{
	unsigned char data[93];///
}__attribute__((packed));
typedef  struct lnt_r_cpu_load_init_req lnt_r_cpu_load_init_req_t;

struct lnt_r_cpu_load_init_ack
{
	unsigned char data[104];
	unsigned short sw;
}__attribute__((packed));
typedef  struct lnt_r_cpu_load_init_ack lnt_r_cpu_load_init_ack_t;


/*
 * R_CPU_LOAD(CPU 钱包圈存)
 */
struct lnt_r_cpu_load_req
{
	unsigned char data[61];///
}__attribute__((packed));
typedef  struct lnt_r_cpu_load_req lnt_r_cpu_load_req_t;

struct lnt_r_cpu_load_ack
{
	unsigned char data[80];
	unsigned short sw;
}__attribute__((packed));
typedef  struct lnt_r_cpu_load_ack lnt_r_cpu_load_ack_t;

/*
 * R_M1_LOAD(M1 充值)
 */
struct lnt_r_m1_load_req
{
	unsigned char data[128];///
}__attribute__((packed));
typedef  struct lnt_r_m1_load_req lnt_r_m1_load_req_t;

struct lnt_r_m1_load_ack
{
	unsigned char data[32];
	unsigned short sw;
}__attribute__((packed));
typedef  struct lnt_r_m1_load_ack lnt_r_m1_load_ack_t;

/*
 * R_M1_CPU_QRY(M1_CPU 钱包查询)
 */
struct lnt_r_m1_cpu_qry_req
{
	unsigned char data[128];///
}__attribute__((packed));
typedef  struct lnt_r_m1_cpu_qry_req lnt_r_m1_cpu_qry_req_t;

struct lnt_r_m1_cpu_qry_ack
{
	unsigned char data[128];///
}__attribute__((packed));
typedef  struct lnt_r_m1_cpu_qry_ack lnt_r_m1_cpu_qry_ack_t;

//////////////////////////////////////////////////////

/*
 * R_DEPOSIT_INIT(押金初始化)
 */
struct lnt_r_deposit_init_req
{
	unsigned char area;          //区域代码
	unsigned char dtype;         //押金类型
	unsigned char mark;  	  //开卡销卡标志
	unsigned short code;       //运营商编码 
	unsigned char member;	   //会员
}__attribute__((packed));
typedef struct lnt_r_deposit_init_req lnt_r_deposit_init_req_t;

struct lnt_r_deposit_init_ack
{
	unsigned char data[176];
}__attribute__((packed));
typedef struct lnt_r_deposit_init_ack lnt_r_deposit_init_ack_t;

/*
 * R_DEPOSIT_PROCESS(押金处理)
 */
struct lnt_r_deposit_process_req
{
	unsigned char data[152];
}__attribute__((packed));
typedef struct lnt_r_deposit_process_req lnt_r_deposit_process_req_t;

struct lnt_r_deposit_process_ack
{
	unsigned char tmark;          		//交易数据标志
	unsigned char comp_tdata[87];    //复合交易数据
	unsigned short sw1sw2;        	//卡片返回码
}__attribute__((packed));
typedef struct lnt_r_deposit_process_ack lnt_r_deposit_process_ack_t;



/******************************************************************
*						岭南通网络指令
********************************************************************/
/*
 * 签到流程指令
 */
#define LNT_REGISTER_STAGE1				0xDD01
#define LNT_REGISTER_STAGE2				0xDD02
#define LNT_REGISTER_STAGE3				0xDD03
#define LNT_REGISTER_STAGE4				0xDD04
#define LNT_REGISTER_STAGE5				0xDD05
#define LNT_REGISTER_STAGE6				0xDD06
#define LNT_REGISTER_STAGE7				0xDD07
#define LNT_REGISTER_STAGE8				0xDD08

//开销卡指令
#define LNT_ON_OFF_CARD				    0xDD73

/*
 * 充值流程指令
 */
#define LNT_RECHARGE_STAGE1				0xDD11
#define LNT_RECHARGE_STAGE2				0xDD12
#define LNT_RECHARGE_STAGE3				0xDD13
#define LNT_RECHARGE_STAGE4				0xDD14
#define LNT_RECHARGE_STAGE5				0xDD15
#define LNT_RECHARGE_STAGE6				0xDD16
#define LNT_RECHARGE_STAGE7				0xDD17
#define LNT_RECHARGE_STAGE8				0xDD18
#define LNT_RECHARGE_STAGE9				0xDD19
#define LNT_RECHARGE_STAGE10			0xDD20
#define LNT_RECHARGE_STAGE11			0xDD21
#define LNT_RECHARGE_STAGE12			0xDD22




struct lnt_packet_head
{
	unsigned short head;
	unsigned char version;
	unsigned short length;
}__attribute__((packed));
typedef struct lnt_packet_head lnt_packet_head_t;


/*
 * 签到阶段1
 */
 struct lnt_packet_register_stage1_CTS
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];	  //PKI卡号
	unsigned char ilen;           //信息长度
	unsigned char INFO[0];      //信息
	unsigned int crc32;         //校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage1_CTS lnt_packet_register_stage1_CTS_t;

/*
 * 签到阶段2
 */
 struct lnt_packet_register_stage2_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[5];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage2_STC lnt_packet_register_stage2_STC_t;

/*
 * 签到阶段3: R-INIT报文
 */
 struct lnt_packet_register_stage3_CTS    
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];	  //PKI卡号
	unsigned char ilen;           //信息长度
	unsigned char R_INIT[14];      //信息(R-INIT返回报文)
	unsigned int crc32;         //校验码	
}__attribute__((packed));
typedef  struct lnt_packet_register_stage3_CTS lnt_packet_register_stage3_CTS_t;

/*
 * 签到阶段4
 */
 struct lnt_packet_register_stage4_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[5];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage4_STC lnt_packet_register_stage4_STC_t;


/*
 * 签到阶段5
 */
 struct lnt_packet_register_stage5
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[8+128+2];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage5 lnt_packet_register_stage5_t;

/*
 * 签到阶段6
 */
 struct lnt_packet_register_stage6
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char R_AC_LOGIN2[46+8+128+2];  	//信息(R_AC_Login2报文)
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage6 lnt_packet_register_stage6_t;

/*
 * 签到阶段7
 */
 struct lnt_packet_register_stage7
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[46+8+128+2];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage7 lnt_packet_register_stage7_t;
  

/*
 * 签到阶段8
 */
 struct lnt_packet_register_stage8
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[0];   //信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_register_stage8 lnt_packet_register_stage8_t;

unsigned int lnt_crc32(unsigned char *buffer, unsigned int size);



/*
 * 开卡、销卡阶段1
 */
struct lnt_open_close_card_stage1
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[176];    //信息
	unsigned int crc32;  	    //校验码
}__attribute__((packed));
typedef  struct lnt_open_close_card_stage1 lnt_open_close_card_stage1_t;


/*
 * 开卡、销卡阶段2
 */
 struct lnt_open_close_card_stage2
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;      //流程指示码
	unsigned char retcode;      //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char R_AC_LOGIN2[152];  
	unsigned int crc32;  	    //校验码
}__attribute__((packed));
typedef  struct lnt_open_close_card_stage2 lnt_open_close_card_stage2_t;


/***************************充值********************************/
/*
 * 充值阶段1
 */
struct recharge_info{
	unsigned char apply_type;//应用类型
	unsigned char recharge_amount[4];//充值金额
	unsigned char order_num[10];//订单号
}__attribute__((packed));
typedef struct recharge_info recharge_info_t;

struct lnt_packet_recharge_stage1_CTS
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];	  //PKI卡号
	unsigned char ilen;           //信息长度
	struct recharge_info INFO;      //信息
	unsigned int crc32;         //校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage1_CTS lnt_packet_recharge_stage1_CTS_t;

/*
 * 充值阶段2
 */
struct lnt_packet_recharge_stage2_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[29];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage2_STC lnt_packet_recharge_stage2_STC_t;

/*
 * 充值阶段3 
 */
struct lnt_packet_recharge_stage3_CTS    
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];	  //PKI卡号
	unsigned char ilen;           //信息长度
	//unsigned char INFO[80];      //51或者80 //
	union info{
		unsigned char Encry_INFO[80+2];//加密
		unsigned char NoEncry_INFO[51+2];//不加密
	}INFO;
	unsigned int crc32;         //校验码	
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage3_CTS  lnt_packet_recharge_stage3_CTS_t;

/*
 * 充值阶段4
 */
 struct lnt_packet_recharge_stage4_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[8+72+5];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage4_STC lnt_packet_recharge_stage4_STC_t;


/*
 * 充值阶段5
 */
 struct lnt_packet_recharge_stage5_CTS 
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[72+2];  	//信息 ///
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage5_CTS  lnt_packet_recharge_stage5_CTS_t;

/*
 * 充值阶段6
 */
 struct lnt_packet_recharge_stage6_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[93];  	/// 
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage6_STC lnt_packet_recharge_stage6_STC_t;

/*
 * 充值阶段7
 */
 struct lnt_packet_recharge_stage7_CTS 
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[104+2];   
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage7_CTS  lnt_packet_recharge_stage7_CTS_t;
  

/*
 * 充值阶段8
 */
 struct lnt_packet_recharge_stage8_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[61];   //信息///
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage8_STC lnt_packet_recharge_stage8_STC_t;


/*
 * 充值阶段9
 */
 struct lnt_packet_recharge_stage9_CTS
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[80+2];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage9_CTS lnt_packet_recharge_stage9_CTS_t;
  

/*
 * 充值阶段10
 */
 struct lnt_packet_recharge_stage10_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[128];   //信息///
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage10_STC lnt_packet_recharge_stage10_STC_t;

/*
 * 充值阶段11
 */
 struct lnt_packet_recharge_stage11_CTS
{
	unsigned char userid[16];   //用户ID
	unsigned char appstep;	    //应用步骤
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	unsigned char INFO[0];  	//信息
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage11_CTS lnt_packet_recharge_stage11_CTS_t;
  

/*
 * 充值阶段12
 */
struct recharge_msg{
	unsigned char recharge_record_id[10];//充值记录ID
	unsigned char pki_num[4];//PKI 卡号
	unsigned char logic_num[8];//逻辑卡号
	unsigned char recharge_amount[4];//充值金额
	unsigned char balance[4];//交易后余额
	
}__attribute__((packed));


struct lnt_packet_recharge_stage12_STC
{
	unsigned char userid[16];   //用户ID
	unsigned char indcode;    //流程指示码
	unsigned char retcode;   //返回信息码
	unsigned char pki[4];    	//PKI卡号
	unsigned char ilen;     	//信息长度
	//unsigned char INFO[30];   //信息///
	struct recharge_msg INFO;
	unsigned int crc32;  	//校验码
}__attribute__((packed));
typedef  struct lnt_packet_recharge_stage12_STC lnt_packet_recharge_stage12_STC_t;

#endif


