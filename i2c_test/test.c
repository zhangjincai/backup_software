
#include <stdio.h>
#include "i2cbusses.h"
#include "i2c-dev.h"
#include "lib_general.h"

#define I2C_BUS			3		//设备所在的总线
#define EEPROM_RW_ADDR	0x64	//设备在i2c总线上的地址

#define	EEPROM_RW_BLOCK_SZ	1

int main(void)
{
	int fd = -1, force = 0, ret = -1;
	char filename[32] = {0};
	
	printf("i2c test......\n");

	fd = open_i2c_dev(I2C_BUS, filename, sizeof(filename), 0);
	if(fd < 0)
	{
		fprintf(stderr, "open i2c dev failed!\n");
		goto ERR;
	}

	force = 1;
	ret = set_slave_addr(fd, EEPROM_RW_ADDR, force);
	if(ret < 0)
	{
		fprintf(stderr, "set i2c slave address failed!\n");
		goto ERR;
	}
	
#if 1
	int data = 0,i,daddress = 0;
	unsigned char block = 0xc9;
	
	ret = i2c_smbus_access(fd,I2C_SMBUS_WRITE,data,I2C_SMBUS_BYTE,NULL);
	lib_msleep(5);
	printf("write ret = %d\n",ret);
	
	//lib_usleep(10);
	//ret = i2c_smbus_read_i2c_block_data(fd, daddress, EEPROM_RW_BLOCK_SZ, block[i]);
	//printf("read ret = %d\n",ret);

#endif	


ERR:
	if(fd)
		close(fd);	
	
	return 0;
}


	
	
	



