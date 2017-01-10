#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#include "lib_general.h"
#include "lib_ctos.h"


#define STAKE_HD_H		0x55
#define STAKE_HD_L		0xaa

static unsigned char g_sn = 0;

static int g_qbl = 1;
//static lib_ctos_t g_ctos1;
//static lib_ctos_t g_ctos2;

static void sigint(int sig)
{
	fprintf(stderr, "singint: %d\n", sig);
		
	system("/sbin/ifconfig can0 down");

	g_qbl = 0;
}

static void signals_init(void)
{
	struct sigaction sa;
	
	sa.sa_flags = 0;
	sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGTERM);
   	sigaddset(&sa.sa_mask, SIGHUP);

	sa.sa_handler = sigint;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = sigint;
	sigaction(SIGTERM, &sa, NULL);
	sa.sa_handler = sigint;
	sigaction(SIGHUP, &sa, NULL);	
}


unsigned char get_sn(void)
{
	if(g_sn > 255)
		g_sn = 0;

	return g_sn++;
}

#define DATA_LEN		(96)
int package_make(unsigned char *data, const unsigned char d_addr, const unsigned char s_addr, const unsigned char cmd)
{
	assert(data);

	unsigned char buf[DATA_LEN] = {0};
	unsigned int len = 0;
	int i;
	unsigned short crc16 = 0;
	unsigned short mlen = DATA_LEN;
	
	for(i = 0; i < DATA_LEN; i++)
	{
		buf[i] = i;
	}


	data[len++] = STAKE_HD_H;
	data[len++] = STAKE_HD_L;
	data[len++] = s_addr;
	data[len++] = d_addr;
	data[len++] = cmd;
	data[len++] = get_sn();
	
	memcpy(&data[len], &mlen, 2);
	len += 2;
	
	memcpy(&data[len], buf, DATA_LEN);
	len += DATA_LEN;

	crc16 = lib_crc16_with_byte(data, len);

	memcpy(&data[len], &crc16, 2);

	len += 2;

	
	fprintf(stderr, "crc16: 0x%02x\n", crc16);
	fprintf(stderr, "len: %d\n", len);


	lib_printf_v2("PACKAGE", data, len, 16);


	return len;
}

int main(int argc, char *argv[])
{
	int ret = -1;
	int i;
	int sec = 0;
	unsigned char d_addr = 0;
	unsigned char s_addr = 0;

	ctos_config_t conf1, conf2;
	lib_ctos_t *ctos1 = NULL, *ctos2 = NULL;
	
	

/*
	g_ctos1.can_bus = 0;;
	g_ctos1.can_buf_num = 10 * 1024;
	g_ctos1.baud_rate = BAUD_RATE_250kbps;
	g_ctos1.rb_sz = 10 * 1024;

	g_ctos2.can_bus = 0;;
	g_ctos2.can_buf_num = 10 * 1024;
	g_ctos2.baud_rate = BAUD_RATE_250kbps;
	g_ctos2.rb_sz = 10 * 1024;
*/

	conf1.can_bus = 0;
	conf1.can_buf_num = 10 * 1024;
	conf1.baud_rate = BAUD_RATE_500kbps;
	conf1.rb_sz = 10 * 1024;

	conf2.can_bus = 0;
	conf2.can_buf_num = 10 * 1024;
	conf2.baud_rate = BAUD_RATE_500kbps;
	conf2.rb_sz = 10 * 1024;

	int len = 0;
	unsigned char data[512] = {0};
	unsigned char buf[512] = {0};

	unsigned int recv_sleep_cnt = 0;
	unsigned int send_err_cnt = 0;
	
	signals_init();

	fprintf(stderr, "%s\n", lib_ctos_version());
	printf("Software Compiled Time: %s, %s.\r\n",__DATE__, __TIME__);
	
	if(argc != 5)
	{
		fprintf(stderr, "WARN: ./ctos [s/c] [s_add] [d_addr] [sec]\n\n");
		return 0;
	}

	for(i = 0; i < argc; i++)
	{
		fprintf(stderr, "%s\n", argv[i]);
	}

	if(strcmp(argv[1], "s") == 0)//read data
	{
		conf1.can_id = atoi(argv[2]);
		s_addr = atoi(argv[2]);	
		d_addr = atoi(argv[3]);	

		fprintf(stderr, "conf1 can id: 0x%02x\n", s_addr);
		//lib_ctos_init(&g_ctos1);
		ctos1 = lib_ctos_create(&conf1);
		if(ctos1 == NULL)
			goto ERR;

		while(1)
		{
			if(!g_qbl)
				goto ERR;
			
			ret = lib_ctos_get(ctos1, buf, 106);
			//fprintf(stderr, "lib_ctos_get ret = %d\n", ret);
			if(ret > 0)
			{
				lib_printf_v2("CTOS", buf, ret, 16);
			}
			else
			{
				recv_sleep_cnt++;
				lib_msleep(500);
			}
		}
	}
	else if(strcmp(argv[1], "c") == 0)//send data
	{
		conf2.can_id = atoi(argv[2]);	
		s_addr = atoi(argv[2]);	
		d_addr = atoi(argv[3]);	
		sec = atoi(argv[4]);
		
		fprintf(stderr, "conf2 can id: 0x%02x\n", conf2.can_id);
		//lib_ctos_init(&g_ctos2);
		ctos2 = lib_ctos_create(&conf2);
		if(ctos2 == NULL)
			goto ERR;
		
		while(1)
		{
			if(!g_qbl)
				goto ERR;

			len = package_make(data, d_addr, s_addr, 0x80);
			if(len > 0)
			{
				ret = lib_ctos_put(ctos2, d_addr, data, len);
				fprintf(stderr, "ctos2 ret = %d\n", ret);
				if(ret < 0)
					send_err_cnt++;
			}
			
			lib_msleep(sec);
		}
	}
		

	while(1)
	{
		if(!g_qbl)
			break;

		lib_sleep(3);
	}

ERR:
	if(strcmp(argv[1], "s") == 0)
	{
		//lib_ctos_detach(&g_ctos1);
		lib_ctos_destroy(ctos1);
		fprintf(stderr, "ctos1\n");
	}
	else if(strcmp(argv[1], "c") == 0)
	{
		//lib_ctos_detach(&g_ctos2);
		lib_ctos_destroy(ctos2);
		fprintf(stderr, "ctos2\n");
	}

	
	fprintf(stderr, "recv_sleep_cnt = %d\n", recv_sleep_cnt);
	fprintf(stderr, "send_err_cnt = %d\n", send_err_cnt);
	
	fprintf(stderr, "\nQuit!\n");
	
	return 0;
}


