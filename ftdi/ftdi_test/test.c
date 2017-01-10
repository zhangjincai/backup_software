#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h> 
#include <signal.h>
#include <errno.h>
#include "lib_general.h"
#include "ftdi.h"


//#define USING_FTDI1
#define USING_FTDI2

static struct ftdi_context *ftdi1, *ftdi2;

static void sigint(int sig)
{
	fprintf(stderr, "signal: %d\n", sig);

#ifdef USING_FTDI1
	ftdi_disable_bitbang(ftdi1);
	ftdi_usb_close(ftdi1);
	ftdi_free(ftdi1);
#endif

#ifdef USING_FTDI2
	ftdi_usb_close(ftdi2);
	ftdi_disable_bitbang(ftdi2);
	ftdi_free(ftdi2);
#endif
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
	sigaddset(&sa.sa_mask, SIGPIPE);

	sa.sa_handler = sigint;
	sigaction(SIGINT, &sa, NULL);
	
	sa.sa_handler = sigint;
	sigaction(SIGTERM, &sa, NULL);
	
	sa.sa_handler = sigint;
	sigaction(SIGHUP, &sa, NULL);	

	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);	
}


int main(int argc, char *argv[])
{
	//signals_init();

	struct ftdi_version_info version;
	
	version = ftdi_get_library_version();

	printf("Initialized libftdi %s (major: %d, minor: %d, micro: %d, snapshot ver: %s)\n",  \
	version.version_str, version.major, version.minor, version.micro, \
	version.snapshot_str);

	struct ftdi_context *ftdi = NULL;
	struct ftdi_device_list *devlist, *curdev;
	int interface = INTERFACE_ANY;
	int ret = 0;
	char manufacturer[512] = {0};
	int mnf_len = 512;
	char description[512] = {0};
	int desc_len = 512;
	char serial[512] = {0};
	int serial_len = 512;
	int retval = 0;
	int i;
#if 1	
	ftdi = ftdi_new();
	if(ftdi == NULL)
	{
		fprintf(stderr, "ftdi_new failed!\n");
		return 0;
	}

	//ftdi_set_interface(ftdi, INTERFACE_B);

	ret = ftdi_usb_find_all(ftdi, &devlist, 0x0403, 0x6010);
	fprintf(stderr, "find ret = %d\n", ret);
#if 0
	if(ret > 0)
	{
		ftdi_usb_get_strings(ftdi, devlist[0].dev, manufacturer, mnf_len, description, desc_len, serial, serial_len);
		fprintf(stderr, "%s\n%s\n%s\n", manufacturer, description, serial);
	}
#endif
	fprintf(stderr, "type: %d\n", ftdi->type);

#if 1
   for(curdev = devlist; curdev != NULL; i++)
   {
        printf("Checking device: %d\n", i);
		
        if ((ret = ftdi_usb_get_strings(ftdi, curdev->dev, manufacturer, 128, description, 128, NULL, 0)) < 0)
        {
	            fprintf(stderr, "ftdi_usb_get_strings failed: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
	            retval = -1;
	            //goto done;
        }
		
        printf("Manufacturer: %s, Description: %s\n\n", manufacturer, description);
      	curdev = curdev->next;
   }
#endif

err:
	if(ftdi != NULL)
	{
		ftdi_free(ftdi);
		ftdi = NULL;
	}
#endif
	
	unsigned char txbuf[512] = {0},rxbuf[512] = {0};

	for(i = 0; i < 32; i++)
	{
		txbuf[i] = i;
	}

#ifdef USING_FTDI1

	ftdi1 = ftdi_new();

	ftdi_set_interface(ftdi1, INTERFACE_A);
	ftdi_usb_open(ftdi1, 0x0403, 0x6010);
	ftdi_set_baudrate(ftdi1, 115200);
	ftdi_set_line_property(ftdi1, BITS_8, STOP_BIT_1, NONE);
	ftdi_setflowctrl(ftdi1, SIO_DISABLE_FLOW_CTRL);
	
	printf("1real baudrate used: %d\n", ftdi1->baudrate);
	
	if (ftdi_set_bitmode(ftdi1, 0xFF, BITMODE_RESET) < 0)
    {
        fprintf(stderr,"Can't set mode: %s\n",ftdi_get_error_string(ftdi1));
   	}
	
#endif

#ifdef USING_FTDI2

	ftdi2 = ftdi_new();
	//ftdi_init(ftdi2);
	
	ftdi_set_interface(ftdi2, INTERFACE_B);
	ftdi_usb_open(ftdi2, 0x0403, 0x6010);
	ftdi_set_baudrate(ftdi2, 115200);
	ftdi_set_line_property(ftdi2, BITS_8, STOP_BIT_1, NONE);
	ftdi_setflowctrl(ftdi2, SIO_DISABLE_FLOW_CTRL);
  	printf("2real baudrate used: %d\n", ftdi2->baudrate);
	if (ftdi_set_bitmode(ftdi2, 0xFF, BITMODE_RESET) < 0)
 	{
     	  fprintf(stderr,"Can't set mode: %s\n",ftdi_get_error_string(ftdi2));
    }
	
	//ftdi_set_bitmode(ftdi1, 0xFF, BITMODE_BITBANG);
	//struct ftdi_device_list *devlist, *curdev;
	//int ret = 0;
	//char manufacturer[512] = {0};
	//int mnf_len = 512;
	//char description[512] = {0};
	//int desc_len = 512;
	//char serial[512] = {0};
	//int serial_len = 512;
	//int retval = 0;
	
	ret = ftdi_usb_find_all(ftdi2, &devlist, 0x0403, 0x6010);
	fprintf(stderr, "find ret = %d\n", ret);
	if(ret > 0)
	{
		ftdi_usb_get_strings(ftdi2, devlist[0].dev, manufacturer, mnf_len, description, desc_len, serial, serial_len);
		fprintf(stderr, "%s\n%s\n%s\n", manufacturer, description, serial);
	}

	fprintf(stderr, "type: %d\n", ftdi2->type);

#if 0
   for (curdev = devlist; curdev != NULL; i++)
    {
	        printf("Checking device: %d\n", i);
			
	        if ((ret = ftdi_usb_get_strings(ftdi, curdev->dev, manufacturer, 128, description, 128, NULL, 0)) < 0)
	        {
		            fprintf(stderr, "ftdi_usb_get_strings failed: %d (%s)\n", ret, ftdi_get_error_string(ftdi));
		            retval = -1;
		            goto done;
	        }
			
	        printf("Manufacturer: %s, Description: %s\n\n", manufacturer, description);
	      	curdev = curdev->next;
    }
#endif
#endif	

#ifdef USING_FTDI1
	while(1)
	{	
	#if 0
		i = ftdi_read_data(ftdi1, rxbuf, 512);
		fprintf(stderr, "ftdi_read_data,i = %d\n", i);
		if(i > 0)
		{
			lib_printf_v2("---ftdi1----", rxbuf, i, 16);
		}
	#endif
		i = ftdi_write_data(ftdi1, txbuf, 32);
		fprintf(stderr, "ftdi_write_data,i = %d\n", i);
		if(i > 0)
		{
			lib_printf_v2("---ftdi1----", txbuf, i, 16);
		}
		
		//memset(txbuf, 0, sizeof(txbuf));
		lib_sleep(1);
	
		fprintf(stderr, "while(1)\n");

	}
#endif

#ifdef USING_FTDI2
	while(1)
	{	
		i = ftdi_read_data(ftdi2, rxbuf, 512);
		fprintf(stderr, "ftdi_read_data,i = %d\n", i);
		if(i > 0)
		{
			lib_printf_v2("---ftdi2----", rxbuf, i, 16);
		}

		memset(rxbuf, 0, sizeof(rxbuf));
		lib_sleep(1);

		fprintf(stderr, "while(1)\n");
	}
#endif

#if 0
	int value = -1;
	ftdi_get_eeprom_value(ftdi2, CHIP_SIZE, &value);
	fprintf(stderr, "CHIP_SIZE value = %d\n", value);

	//ftdi_disable_bitbang(ftdi1);

	ftdi_usb_close(ftdi1);
	//ftdi_usb_close(ftdi2);
	
	ftdi_free(ftdi1);
//	ftdi_free(ftdi2);
#endif

#ifdef USING_FTDI1
	ftdi_disable_bitbang(ftdi1);
	ftdi_usb_close(ftdi1);
	ftdi_free(ftdi1);
#endif

#ifdef USING_FTDI2
	ftdi_usb_close(ftdi2);
	ftdi_disable_bitbang(ftdi2);
	ftdi_free(ftdi2);
#endif

	return 0;
}



