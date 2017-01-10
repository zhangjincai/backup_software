
#include "lib_general.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc,char *argv[])
{
	int ret = 0xff;
/*	char *ver;

	printf("hello world..........\n");
	ver = lib_general_version();
	printf("%d\n",*ver);
*/	
  	lib_serial_t client;
  	memset(&client, 0, sizeof(lib_serial_t));
  
  	strcpy(client.pathname,"/dev/ttyo3");
  	client.flags = O_RDWR;
  	client.speed = 115200;
  	client.databits = 8;
  	client.stopbits = 1;
      	client.sfd = open("/dev/ttyo3",O_RDWR);
	printf("sfd:%d\n",client.sfd);	 

 	ret = lib_serial_init(&client);
	
	unsigned short crc16;
	
	char sbuf[1024];
	sbuf[0] = 0x55;
	sbuf[1] = 0xaa;
	sbuf[2] = 0x87;//视频监控系统电源控制
	sbuf[3] = 0x01;
	sbuf[4] = 0x01;
	
	crc16 = (unsigned short)lib_crc16_with_table(&sbuf[3], 2);

	strncpy(&sbuf[5],(char *)&crc16,2);
  	ret = lib_serial_send(client.sfd,sbuf,sizeof(sbuf));
	printf("send ret:%d\n",ret);
  
  	lib_serial_close(&client);
	
	return 0;
}
