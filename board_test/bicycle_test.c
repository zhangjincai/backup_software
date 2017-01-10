#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

#include "include/bicycle.h"
#include "include/lib_general.h"

#define DEV_NAME  "/dev/ttyO1"

lib_serial_t serial;
int ret = -1;

int main()
{
  unsigned char rxbuf[1024];
  unsigned char txbuf[1024];
  int i = 1;
  unsigned int sn32 = 1;

  //__signals_init();	

  strcpy(serial.pathname,DEV_NAME);
  serial.flags = O_RDWR;
  serial.speed = 115200;
  serial.databits = 8;
  serial.stopbits = 1;

  //init the serial
  ret = lib_serial_init(&serial);
  if(ret<0)
  {
	printf("************lib_serial_init************%s\n",DEV_NAME);
	exit(0);
  }

	memset(txbuf,0,sizeof(1024));

#if 1
while(1)
{
	memset(rxbuf,0,sizeof(rxbuf));
	printf("-------------\n");
	ret = lib_serial_readn_select(serial.sfd, rxbuf, 7, 1500);
	printf("-------------\n");
	if(ret<=0)
	{
		printf("the %d time!\n*******lib_serial_recv_select is NULL!----%s\n\n",i++,DEV_NAME);
	}else{
		lib_printf("rxbuf",rxbuf,7);

		ret = analysts(rxbuf,&serial);
		if(ret<0)
		{
			printf("************analysts error!************%s\n",DEV_NAME);

			ret = lib_serial_send(serial.sfd, txbuf, 7);
			if(ret<=0)
			{
				printf("************lib_serial_send error!************\n");
				
			 }else
				lib_printf("txbuf",txbuf,7);
		}
	}

	
}
  
#endif



	lib_serial_close(&serial);

	printf("Program Terminated\n");

	return 0;
}

