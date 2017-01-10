
#include "lib_general.h"
#include <stdio.h>
#include <fcntl.h>

int main(int argc,char *argv[])
{
	int ret = -1;
	char *ver;
	
	printf("hello world..........\n");

	ver = lib_general_version();
	printf("%c\n",ver);
	
  	/*lib_serial_t server;
  	memset(&server, 0, sizeof(lib_serial_t));
  
  	ret = strcpy(server.pathname,"/dev/ttyo3");
  	server.flags = O_RDWR;
  	server.speed = 115200;
  	server.databits = 8;
  	server.stopbits = 1;
  	
  
  	char buf[] = "hello world";
  
  	server.sfd = lib_serial_init(&server);
  	ret = lib_serial_send(server.sfd,buf,strlen(buf));
  
  
  	lib_serial_close(&server);*/
	
	return 0;
}
