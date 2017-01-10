/* serial_test.c
   Read/write data via serial I/O
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include "ftdi.h"

static int exitRequested = 0;

int main(int argc, char **argv)
{
    struct ftdi_context *ftdi;
    unsigned char rbuf[1024] = {0};
    int f = 0, i;
    int vid = 0x0403;//
    int pid = 0x6010;//
    int baudrate = 115200;
    int interface = INTERFACE_A;//INTERFACE_ANY;
    int do_write = 1;//读写标志
    unsigned int pattern = 0xf0;//要写的数据
    int retval = EXIT_FAILURE;

	unsigned char tbuf[] = {0x01,0x02,0x03,0x04};
	
    // Init
    if ((ftdi = ftdi_new()) == 0)
    {
        fprintf(stderr, "ftdi_new failed\n");
        return EXIT_FAILURE;
    }

    // Select interface
    ftdi_set_interface(ftdi, interface);
    
    // Open device
    f = ftdi_usb_open(ftdi, vid, pid);
    if (f < 0)
    {
        fprintf(stderr, "unable to open ftdi device: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }

    // Set baudrate
    f = ftdi_set_baudrate(ftdi, baudrate);
    if (f < 0)
    {
        fprintf(stderr, "unable to set baudrate: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }
    
    //Set line parameters
    f = ftdi_set_line_property(ftdi, 8, STOP_BIT_1, NONE);
    if (f < 0)
    {
        fprintf(stderr, "unable to set line parameters: %d (%s)\n", f, ftdi_get_error_string(ftdi));
        exit(-1);
    }

    while(!exitRequested)
    {
        if(do_write)
			f = ftdi_write_data(ftdi, tbuf, 4);
        else
            f = ftdi_read_data(ftdi, rbuf, 128);
        if(f < 0)
            sleep(1);
        else if(f > 0 && !do_write)
        {
            fprintf(stderr, "read %d bytes\n", f);
			for(i = 0; i < f; i++)
				printf("%02x ",rbuf[i]);
			printf("\n");
            //fwrite(rbuf, f, 1, stdout);
            fflush(stderr);
            fflush(stdout);
        }
    }
    
    retval =  EXIT_SUCCESS;
            
    ftdi_usb_close(ftdi);
    ftdi_free(ftdi);

    return retval;
}
