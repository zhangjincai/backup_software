
#include <stdio.h>
#include <assert.h>
#include "common.h"
#include "lib_general.h"


int package_make(unsigned char *tbuf, const unsigned char can_addr, const unsigned char ctrl_bit, const unsigned char cmd)
{
	assert(tbuf);

	int ret = 0, len = 0;
	unsigned char data[128] = {0};
	unsigned short crc16; 
	unsigned short node_num = htons(0xAAAD),data_len;

	//unsigned char buf[3] = {0x0,0x0,0x0a};//桩机总数量
	unsigned char buf[DATA_LEN] = {0x54,0x4A,0x16,0xC9,0xF1,0x3A, 0xAA,0xAD, 0x00,0x01, 0x00,0x01};//my
	unsigned char txbuf[512] = {0};

	//ret = lib_b64_encode_hex(txbuf,buf,3);//base64编码
	ret = lib_b64_encode_hex(txbuf,buf,DATA_LEN);//base64编码
	//printf("--------->ret = %d\n",ret);
	
	data_len = htons(ret + 2);//不是均衡负载申请的话需要再+1
		
	data[len++] = FRAME_HEAD;//帧头
	data[len++] = 0x02;//流水号,0~255
	memcpy(&data[len],&node_num,2);//节点机编号
	len += 2;
	
	data[len++] = can_addr;//设备地址
	data[len++] = ctrl_bit;//控制位
	data[len++] = cmd;//类别引导字

	memcpy(&data[len],&data_len,2);//数据长度
	len += 2;

	data[len++] = 0x44;//0x61;//引导字D
	data[len++] = 0x50;//0x69;//引导字P
	
	memcpy(&data[len],&txbuf,ret);//数据体
	len += ret;

	//data[len++] = 0x26;//& //仅均衡负载申请不需要 &

	crc16 = htons((unsigned short)lib_crc16_with_byte(&data[1],len-1));//校验
	memcpy(&data[len],&crc16,2);
	len += 2;
	
	data[len++] = FRAME_TAIL;//帧尾

	ret = __uplink_escape(tbuf,data,len);//转义

	//fprintf(stderr,"packet len:%d\n",ret);
	lib_printf_v2("-----------PACKAGE-------------", data, len, 16);

	return ret;
}


int data_frame_init(data_frame_t *data)
{
	data->head = 0xf1;
	data->serial_num = 0x01;
	data->term_code = htons(0xaaad);
	data->dev_addr = 0x00;
	data->ctrl_bit = 0x02;
	data->tail = 0xf2;
	
	return 0;
}


