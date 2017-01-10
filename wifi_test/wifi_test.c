#include <stdio.h>
#include "lib_general.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define	DEVNAME	"/dev/ttyUSB0"

lib_serial_t uart1;

unsigned char CheckSum(unsigned char *data,const unsigned char len);
int uart_send(void);

int main(void)
{
	int ret = -1;
	ret = open(DEVNAME,O_RDWR);
	if(ret == -1)
	{
		perror("open");
		return -1;
	}
	
#if 1
	printf("wifi testing.......\n");

	//int ret = -1;
	char tbuf[128] = {0},rbuf[128] = {0};
	//lib_serial_t uart1;
  	memset(&uart1, 0, sizeof(lib_serial_t));
  	//串口初始化
  	strcpy(uart1.pathname,DEVNAME);
  	uart1.flags = O_RDWR;
  	uart1.speed = 57600;
  	uart1.databits = 8;
  	uart1.stopbits = 1;
		 
 	ret = lib_serial_init(&uart1);
	if(ret == -1)
		perror("serial init");
	printf("uart init ret:%d,sfd:%d\n",ret, uart1.sfd);
#endif

	//uart_send();

	unsigned char data[128] = {0}, rdata[128] = {0};

	strcpy(&data,"AT+UART");
	data[strlen("AT+UART")] = 0x0d;

	//printf("%s\n",data);
	ret = lib_serial_send(uart1.sfd,&data,strlen(data));
	printf("send ret = %d\n",ret);

	ret = lib_serial_read_select(uart1.sfd,&rdata,sizeof(rdata),5000);
	//ret = lib_serial_recv_select(&uart1,&rdata,sizeof(rdata),5000);
	printf("read ret = %d\n",ret);
	
	return 0;
}

int uart_send(void)
{
	int ret = -1,len = 0;
	char tbuf[128] = {0},port[2] = {0x17,0x0},addr[4] = {0x07,0x0,0xa8,0xc0},data[] = {0x0};
	
	tbuf[len++] = 0x55;//header
	tbuf[len++] = 0xaa;
	tbuf[len++] = 0x00;//len
	tbuf[len++] = 0x0a;
	tbuf[len++] = 0x02;//function word
	tbuf[len++] = 0x00;//备用参数区
	tbuf[len++] = 0x00;
	memcpy(&tbuf[len],port,2);//目标端口
	len += 2;
	memcpy(&tbuf[len],addr,4);//目标地址
	len += 4;
	memcpy(&tbuf[len],data,sizeof(data));//data
	len += sizeof(data);
	tbuf[len] = CheckSum(&tbuf[4], len-4);//checksum

	ret = lib_serial_send(uart1.sfd, &tbuf, len);
	lib_vprintf("send %d bytes\n",ret);
	lib_printf("------send------\n", tbuf, 4);

	return 0;
}

//校验码
unsigned char CheckSum(unsigned char *data,const unsigned char len)
{
	int i;
	unsigned char check;
	for(i = 0; i < len; i++)
	{
		check += data[i];
	}

	return check;
}



















