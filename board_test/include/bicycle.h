#ifndef __BICYCLE_TEST_H__
#define __BICYCLE_TEST_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>


#include "lib_general.h"

#define GPS_DEV_NAME  "/dev/ttyO4"
#define WIFI_DEV_NAME  "/dev/CH340G"

#define true 1
#define false 0

int analysts(unsigned char *rxbuf, lib_serial_t *serial);
unsigned char crc16(unsigned char *buf, unsigned short len);
int checkConnect(char *dst, int cnt);
int package_make(unsigned char *data, const unsigned char d_addr, const unsigned char s_addr, const unsigned char cmd);
void __signals_init(void);	
void __sigint(int sig);


void gps_test(lib_serial_t *serial);
void wifi_test(lib_serial_t *serial);
void thrg_test(lib_serial_t *serial);
void can_test(lib_serial_t *serial);
void rj45_test(lib_serial_t *serial);
void ftdi_test(lib_serial_t *serial);
void i2c_test(lib_serial_t *serial);

void encryption_test(lib_serial_t *serial);
void ttyO2_test(lib_serial_t *serial);




#endif

