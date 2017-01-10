
#include "lib_general.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>


#define	DEVNAME	"/dev/ttyO4"

int main(int argc,char *argv[])
{
	//lib_begin_runtime_printf();
	
	int ret = -1,i,j;
	char *ver,time[64];

	ver = lib_general_version();//获取通用库版本
	lib_vprintf("lib version:%s\n",ver);

	lib_general_last_modify_datetime(time);//获取通用库最后修改时间
	lib_vprintf("last change time:%s\n",time);

  	lib_serial_t server;
  	memset(&server, 0, sizeof(lib_serial_t));
  
  	strcpy(server.pathname,DEVNAME);
  	server.flags = O_RDWR;
  	server.speed = 38400;
  	server.databits = 8;
  	server.stopbits = 1;

 	ret = lib_serial_init(&server);//初始化函数中已包含sfd的定义
 	lib_vprintf("init ret:%d\n",ret);
	char rbuf[512] = {0};
	
	while(1)
	{
		//ret = lib_serial_read_select(server.sfd,rbuf, 512, 200);
	  	ret = lib_serial_readn_select(server.sfd,rbuf, 512, 200);//串口接收
		//ret = lib_serial_recv_select(&server,rbuf,512,200);
		//ret = lib_serial_recv(server.sfd,rbuf,512);
		lib_vprintf("recv %d bytes\n",ret);
	   
		for(i = 0, j = 1; i < ret; i++, j++)
		{
			lib_vprintf("%2x ",rbuf[i]);
			if(j % 20 == 0)
				lib_vprintf("\n");
		}
		lib_vprintf("\n");
	}
	
  	lib_serial_close(&server);

	return 0;
}
