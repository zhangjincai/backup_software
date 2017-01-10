
#include "lib_general.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define	DEVNAME	"/dev/ttyO3"

int main(int argc,char *argv[])
{
	int ret = -1,i;
	char *ver;

	fprintf(stderr, "UART test software compiled time: %s, %s.\r\n",__DATE__, __TIME__);

	ver = lib_general_version();//获取通用库版本
	printf("lib version:%s\n",ver);
	
  	lib_serial_t server;
  	memset(&server, 0, sizeof(lib_serial_t));
  
  	strcpy(server.pathname,DEVNAME);
  	server.flags = O_RDWR;
  	server.speed = 9600;
  	server.databits = 8;
  	server.stopbits = 1;
		 
 	ret = lib_serial_init(&server);
	printf("init ret:%d:%d\n",ret, server.sfd);
	printf("sfd:%d\n",server.sfd);
	
	unsigned short crc16;
	char sbuf[256] = {0},rbuf[256] = {0},verson[128] = {0};
	sbuf[0] = 0x55;
	sbuf[1] = 0xaa;//帧头

	char ch;
	printf("please input msg to send (cmd datalen data):\n");
	while((ch = getchar()) != '\n')
	{
		scanf("%x %x %x",&sbuf[2],&sbuf[3],&sbuf[4]);
	}
	
	crc16 = (unsigned short)lib_crc16_with_table(&sbuf[3], 2);//crc16校验
	//crc16 = (unsigned short)lib_crc16_with_byte(&sbuf[3], 2);
	//crc16 = (unsigned short)lib_crc16_with_bit(&sbuf[3], 2);
	printf("%x\n",crc16);

	memcpy(&sbuf[5],&crc16,2);
	//交换校验码的两个字节
	if(sbuf[3] == 1)
	{
		sbuf[5] += sbuf[6];
		sbuf[6] = sbuf[5] - sbuf[6];
		sbuf[5] = sbuf[5] - sbuf[6];
	}
	else if(sbuf[3] == 0)
	{
		sbuf[4] += sbuf[5];
		sbuf[5] = sbuf[4] - sbuf[5];
		sbuf[4] = sbuf[4] - sbuf[5];
	}
	
	lib_printf("------send------\n", sbuf, 7);

	while(1)
	{
	  	ret = lib_serial_send(server.sfd,sbuf,7);//串口发送
	  	//ret =  lib_serial_send_select(&server,rbuf, 7, 200);
		printf("send  %d bytes\n",ret);
		
		if(sbuf[2] == 0x85)//读取温度
			lib_sleep(3);
		else
			lib_sleep(1);
		
	  	ret = lib_serial_readn_select(server.sfd,rbuf, 128, 1000);//串口接收
	  	printf("recv %d bytes\n",ret);
		
		if(sbuf[2] != 0x8b)
	  		lib_printf("------recv------\n", rbuf, ret);
		else//读取软件版本号
		{
			printf("software version:");
			for(i = 0; i < 8; i++)
				printf("%d", rbuf[27+i]);
			printf("\n");
		}
	  	//ret = lib_serial_recv_select(&server,rbuf, 128, 1000);
	  	//ret = lib_serial_recv(server.sfd,rbuf,128);
	}
  	lib_serial_close(&server);
	
	return 0;
}
