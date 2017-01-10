
#include "lib_general.h"
#include "lib_ctos.h"
#include <stdio.h>
#include <assert.h>

#define 	STAKE_HD_H  0x55
#define 	STAKE_HD_L  0xaa

static unsigned char g_sn = 0;
const unsigned char d_addr = 0x7f;
const unsigned char s_addr = 0x00;

unsigned char get_sn(void)
{
	if(g_sn > 255)
		g_sn = 0;

	return g_sn++;
}

#define DATA_LEN		(8)
int package_make(unsigned char *data, const unsigned char d_addr, const unsigned char s_addr, const unsigned char cmd)
{
	assert(data);

	unsigned short crc16;
	unsigned short data_len = DATA_LEN;
	unsigned char i,len = 0;
	unsigned char buf[DATA_LEN] = {0};

	buf[0] = 0x61;//a
	buf[1] = 0x63;//c
	buf[2] = 0x30;//0
	buf[3] = 0x26;//&

	data[len++] = STAKE_HD_H;
	data[len++] = STAKE_HD_L;
	data[len++] = s_addr;
	data[len++] = d_addr;
	data[len++] = cmd;
	data[len++] = get_sn();
	memcpy(&data[len],&data_len,2);//data len
	len += 2;
	memcpy(&data[len],buf,DATA_LEN);//data
	len += DATA_LEN;
	crc16 = (unsigned short)lib_crc16_with_byte(data,len);
	memcpy(&data[len],&crc16,2);
	len += 2;

	fprintf(stderr,"crc16:0x%02x\n",crc16);
	fprintf(stderr,"data len:%d\n",len);
	lib_printf_v2("PACKAGE", data, len, 16);

	return len;
}
/////////////////////////////////////////
int main(int argc,char *argv[])
{
	int ret = -1,len = 0;
	unsigned char buf[512] = {0},data[512] = {0};
	char *version = NULL;
	
	version = lib_ctos_version();
	lib_vprintf("%s\n",version);
	printf("Software Compiled Time:%s,%s.\r\n",__DATE__,__TIME__);
	
	ctos_config_t conf;
	lib_ctos_t *ctos;
	conf.can_bus = 1;//can总线号
	conf.can_id = 0; //can id号
	conf.baud_rate = BAUD_RATE_125kbps;
	conf.can_buf_num = 5*1024;
	conf.rb_sz = 10*1024;
	
	ctos = lib_ctos_create(&conf);	
	if(NULL == ctos)
	{
		lib_vprintf("Create ctos failed....\n");
		goto error;
	}

	while(1)
	{
#if 1

		len = package_make(data, d_addr, s_addr, 0x82);
		lib_printf_v2("data:", data, len, 16);
		if(len > 0)
		{
			ret = lib_ctos_put(ctos,d_addr,data,len);
			printf("put ret:%d.\n",ret);
		}
		lib_msleep(10000);
		//while(1){
			ret = lib_ctos_get(ctos, buf, 512);
			if(ret > 0)
			{
				lib_printf_v2("recv:", buf, ret, 16);//16:满16个换行
				fprintf(stderr, "%s\n",&buf[8]);
			}
		//}
		lib_msleep(3000);
#endif
	}
	fprintf(stderr,"test end.....\n");
	
error:
	lib_ctos_destroy(ctos);	
	return 0;
}




