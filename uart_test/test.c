#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <stdarg.h>
#include <string.h>

#include "includes/lib_general.h"
//#include "lib_zmalloc.h"



static lib_serial_t g_serial;

static void sigint(int sig)
{
	fprintf(stderr, "signal: %d\n", sig);

	lib_serial_close(&g_serial);

	exit(1);
}

static void signals_init(void)
{
	struct sigaction sa;

	signal(SIGPIPE, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	
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



int main(int argc, char *argv[])
{

	//fprintf(stderr, "UART test software compiled time: %s, %s.\r\n",__DATE__, __TIME__);

	//signals_init();
	
	int n = 0;
	int err = LIB_GE_ERROR;

	unsigned char txbuf[512] = {0};
	unsigned char rxbuf[512] = {0};
	
	memset(&g_serial, 0, sizeof(lib_serial_t));

	strcpy(g_serial.pathname, argv[1]);

	fprintf(stderr, "%s\n", argv[1]);

	g_serial.flags = O_RDWR;
	g_serial.speed = 9600;
	g_serial.databits = 8;
	g_serial.stopbits = 1;

	err = lib_serial_init(&g_serial);
	fprintf(stderr, "%s: err = %d\n", __FUNCTION__, err);

	txbuf[0] = 0x55;
	txbuf[1] = 0xaa;
	txbuf[2] = 0x8b;
	txbuf[3] = 0x00;
	txbuf[4] = 0x00;
	txbuf[5] = 0x00;
	//txbuf[5] = 0x33;
	//txbuf[6] = 0x31;

	 unsigned short crc16;
	 crc16 = (unsigned short)lib_crc16_with_table(&txbuf[3],2);
	 printf("=============crc16:%04x\n",crc16);
	
	//while(1)
	{		
		n = lib_serial_send(g_serial.sfd, txbuf, 6);
		lib_printf("________serial send____________\n", txbuf, n);
		lib_sleep(2);
		n = lib_serial_recv(g_serial.sfd, rxbuf, 128);
		//n = lib_serial_readn_select(g_serial.sfd, rxbuf, 7, 1000);
		fprintf(stderr, "read ret = %d\n", n);
		if(n > 0)
		{
			lib_printf_v2("________serial recv____________\n", rxbuf, n, 16);
			memset(rxbuf, 0, sizeof(rxbuf));
		}

		//lib_sleep(1);
	}


	lib_serial_close(&g_serial);
	
	
	return 0;
}






