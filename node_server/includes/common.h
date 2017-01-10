
#ifndef __COMMON_H__
#define __COMMON_H__


#define 	FRAME_HEAD	0xf1
#define 	FRAME_TAIL	0xf2
#define 	DATA_LEN	(12)
#define 	DAT_LEN	(23)


typedef	 unsigned char uchar;
typedef  unsigned int uint;
typedef  unsigned short ushort;

struct dev_register_req{
	uchar mac_addr[6];
	uchar encry_key[16];
}__attribute((packed));
typedef struct dev_register_req dev_register_req_t;

struct dev_register_ack{
	uchar status;
	uchar server_time[7];
}__attribute((packed));
typedef struct dev_register_ack dev_register_ack_t;


//通信数据帧引导字定义
#define	DEV_REGISTER		0xA1 //设备签到
#define	DEV_CTRL			0xA2 //设备控制
#define	FILE_SEND			0xA3 //泛文件传输
#define	LOAD_BALANCE_REQ 	0xA4 //负载均衡申请
#define	PASS_SEND			0xA5 //数据透传 

struct load_balance_req{
	uchar mac_addr[6];	//mac地址
	ushort dev_code;	//设备编号
	ushort operator_code;//运营商编码
	ushort dev_type;	//设备类型
}__attribute__((packed));
typedef struct load_balance_req load_balance_req_t;


struct load_balance_ack{
	uchar status;	//执行状态
	uchar com_ip[8];//通讯机ip
	ushort com_port;//通讯机端口
	uchar encry_key[16];//加密密钥
}__attribute((packed));
typedef struct load_balance_ack load_balance_ack_t;

//
struct data_frame{
	uchar head;//帧头
	uchar serial_num;//流水号
	ushort term_code;//终端编号
	uchar dev_addr;//设备地址
	uchar ctrl_bit;//控制位
	uchar cmd;//类别命令字
	ushort data_len;//数据长度
	ushort guid_word;//引导字
	uchar data[128];
	ushort crc16;//校验
	uchar tail;//帧尾
}__attribute((packed));
typedef struct data_frame data_frame_t;


//泛文件头
struct file_header{
	ushort fnum;//文件序号
	ushort packet_sum;//分割总数
	ushort packet_num;//分割序号
	uchar  ftype;//文件类别
	uchar  feature[9];//特征定义
}__attribute((packed));
typedef struct file_header file_header_t;





int package_make(unsigned char *tbuf, const unsigned char can_addr, const unsigned char ctrl_bit, const unsigned char cmd);	
//int make_package(unsigned char *tbuf, const unsigned char can_addr, const unsigned char ctrl_bit, const unsigned char cmd);



#endif /*__COMMON_H__*/

