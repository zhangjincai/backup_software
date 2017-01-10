#ifndef __WIFI_H__
#define __WIFI_H__

/*
 * 头文件
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#include "lib_general.h"

/*
 * 函数返回码定义
 */
 
#define WIFI_NULL				(NULL) 
#define WIFI_EOK				(0)
#define WIFI_ERROR     			(-1)
#define WIFI_ETIMEOUT			(-2) //超时
#define WIFI_EFULL				(-3) //满
#define WIFI_EEMPTY				(-4) //空
#define WIFI_ENOMEM 			(-5) //内存不够
#define WIFI_EXCMEM				(-6) //内存越界
#define WIFI_EBUSY				(-7) //忙

#define LIB_GE_TRUE				(1)
#define LIB_GE_FALSE			(0)

#define msleep(x) usleep(x*1000)


// struct
struct wifi_netp{

	unsigned char protocol[4];
	unsigned char type[7];
	unsigned short port;
	unsigned char ipaddr[32];
}__attribute__((packed));

struct wifi_lann{

	unsigned char lan_ip[32];
	unsigned char lan_mask[32];
}__attribute__((packed));

struct wifi_wakey{

	unsigned char auth[7];
	unsigned char encry[8];
	unsigned char key[64];
}__attribute__((packed));

struct wifi_wap{

	unsigned char wifi_mode[6];
	unsigned char ssid[128];
	unsigned char channel[5];
}__attribute__((packed));

struct wifi_wmode{

	unsigned char wmode[4];
}__attribute__((packed));


// functions
int  invalid_ipaddr(char *str);
int __wifi_init(lib_serial_t *serial);
void __wifi_resease(lib_serial_t *serial);
int __init_AT_mode(int fd);
int __sendAT(int fd, const unsigned char *cmd,unsigned char *rxbuf);

int wifi_get_lann(int fd, struct wifi_lann *lann);
int wifi_set_lann(int fd, struct wifi_lann *lann);
int Showinfo_lann(struct wifi_lann *lann);
int wifi_get_netp(int fd, struct wifi_netp *netp);
int wifi_set_netp(int fd, struct wifi_netp *netp);
int Showinfo_netp(struct wifi_netp *netp);
int wifi_get_wakey(int fd, struct wifi_wakey *wakey);
int wifi_set_wakey(int fd, struct wifi_wakey *wakey);
int Showinfo_wakey(struct wifi_wakey *wakey);
int wifi_get_wap(int fd, struct wifi_wap *wap);
int wifi_set_wap(int fd, struct wifi_wap *wap);
int Showinfo_wap(struct wifi_wap *wap);

int wifi_get_wmode(int fd, struct wifi_wmode *wmode);
int wifi_set_wmode(int fd, struct wifi_wmode *wmode);
int Showinfo_wmode(struct wifi_wmode *wmode);

int wifi_get_ip(int fd, unsigned char *ip);
int wifi_set_ip(int fd, unsigned char *ip);
int wifi_get_port(int fd, unsigned short *port);
int wifi_set_port(int fd, unsigned short port);
int wifi_get_key(int fd, unsigned char *key);
int wifi_set_key(int fd, unsigned char *key);
int wifi_get_ssid(int fd, unsigned char *ssid);
int wifi_set_ssid(int fd, unsigned char *ssid);
int wifi_get_ssid(int fd, unsigned char *ssid);
int wifi_set_ssid(int fd, unsigned char *ssid);

#endif


