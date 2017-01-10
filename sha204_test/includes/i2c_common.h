#ifndef __I2C_COMMON_H__
#define __I2C_COMMON_H__


int i2c_dev_init(const char *bus_dev, const char *dev_addr);
int read_chip(int fd,char buff[],int addr,int count);
int write_chip(int fd,char buff[],int addr,int count);


#endif/*#ifndef __I2C_COMMON_H__*/

