#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "lib_general.h"


#define	DEV_NAME  "/dev/ftdi1" //"/dev/ttyUSB1"

int main(int argc,char *argv[])
{
	int fd = -1,ret = -1;
	lib_serial_t ftdi;

	fd = open(DEV_NAME,O_RDWR);	
	if(fd == -1){		
		perror("open");		
		return -1;	
	}
 	
	//´®¿Ú³õÊ¼»¯
	memset(&ftdi, 0, sizeof(lib_serial_t)); 
	strcpy(ftdi.pathname,DEV_NAME);  	
	ftdi.flags = O_RDWR;  	
	ftdi.speed = 115200;  	
	ftdi.databits = 8;  	
	ftdi.stopbits = 1;		  	
	
	ret = lib_serial_init(&ftdi);	
	if(ret == -1){		
		perror("serial init");	
		return -1;
	}
	printf("uart init success! ret:%d,sfd:%d\n",ret, ftdi.sfd);

	unsigned char tbuf[] = {0x01,0x02,0x03,0x04},rbuf[128] = {0};

	while(1){
	#if 1
		ret = lib_serial_send(ftdi.sfd, &tbuf, 4);
		printf("send ret = %d\n",ret);
		lib_printf("------send------\n", tbuf, 4);
	#endif
		lib_sleep(1);
		
		ret = lib_serial_recv(ftdi.sfd, &rbuf, 128);
		printf("read ret = %d\n",ret);
		lib_printf("------read------\n", rbuf, ret);
	}

	close(fd);
	return 0;
}
