#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <string.h>
#include <errno.h>
#include "i2c_common.h"

#define PAGE_SIZE 128 

/*i2c设备初始化*/
int i2c_dev_init(const char *bus_dev, const char *dev_addr)
{
    int fd = -1;
    
    fd = open(bus_dev, O_RDWR);
	if (fd < 0) 
	{
		//perror("open");
		return -1;
	}

	if (ioctl(fd, I2C_SLAVE_FORCE, dev_addr) < 0) 
	{	 
		//printf("ioctl:set slave address failed!\n"); 
        return -2;
	}
 
    return fd;
}

/*i2c写函数*/
int write_chip(int fd,char buff[],int addr,int count)
{
	int ret;
	char sendbuffer[PAGE_SIZE+1];

	memcpy(sendbuffer+1,buff,count);
	sendbuffer[0] = addr;
	ret = write(fd,sendbuffer,count+1);

	return ret;
}

/*i2c读函数*/
int read_chip(int fd,char buff[],int addr,int count)
{
	int ret;
	if(write(fd,&addr,1)!=1)	 
		return -1;
	ret = read(fd,buff,count);
	
	return ret;
}

