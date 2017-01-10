#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <string.h>
#include "sha204_command.h"
#include "LIB_SHA204.h"


/*唤醒sha204*/
int sha204_wakeup(int fd)
{
    int ret = -1, addr = 0;
    unsigned char wrdata = 0, rddata[WAKEUP_RSP_SIZE] = {0};

#if 1//wake up
	write_chip(fd,&wrdata,addr,1);
	lib_msleep(1);
#endif

#if 1//wakeup response
	ret = read_chip(fd,rddata,addr,WAKEUP_RSP_SIZE);
	if(ret != WAKEUP_RSP_SIZE)
	{
		printf("read error ...\n");
		return -1;
	}

	lib_printf_v2("wakeup response:",&rddata,ret,16);
    if(rddata[1] != 0x11)//status error!
    {
    	return -2;
    }
#endif
	
	lib_msleep(1);
    return 0;
}


/*产生随机数*/
/*mode 0: update seed; 1: no update seed*/
int sha204_random(int fd,unsigned char *rxbuf,unsigned char mode)
{
#if 1
	int len = 0, ret = -1, addr = 0;
	unsigned char buf[MAX_BUF_SIZE] = {0}, rddata[RANDOM_RSP_SIZE] = {0};
	unsigned short crc16;
	
	buf[len++] = SHA204_NORMAL;//cmd
	buf[len++] = MIN_PAKET_SIZE;//len
	buf[len++] = SHA204_RANDOM;//op
	buf[len++] = mode;//arg1
	buf[len++] = 0x00;//arg2
	buf[len++] = 0x00;//arg2

	sha204_calculate_crc(len - 1,&buf[1],&crc16);//must use this!
	memcpy(&buf[len],&crc16,PACKET_CHECKSUM_SIZE);
	len += PACKET_CHECKSUM_SIZE;

	ret = write_chip(fd,&buf,addr,len);
	//printf("write ret = %d\n",ret);
	if(ret != len + 1)
	{
		printf("write error ...\n");
		return -1;
	}
	
	//lib_printf_v2("write data:",&buf,ret - 1,16);
	lib_msleep(100);
#endif

#if 1//random command response
	memset(&rddata,0,RANDOM_RSP_SIZE);
	ret = read_chip(fd,&rddata,addr,RANDOM_RSP_SIZE);
	if(ret != RANDOM_RSP_SIZE)
	{
		printf("read error ...\n");
		return -2;
	}

    memcpy(rxbuf,&rddata[1],RANDOM_NUM_SIZE);
	//lib_printf_v2("random num:",&rddata,ret,16);
#endif

	lib_msleep(1);
	return 0;
}

/*mode: mode selects message fields
 *slot_id: key_id slot index of key
 *challenge: challenge pointer to challenge (not used if mode bit 0 is set)*/
int sha204_mac(int fd,unsigned char *rxbuf,unsigned char mode,unsigned short slot_id,unsigned char *challenge)
{
#if 1
	int len = 0, ret = -1, addr = 0;
	unsigned char buf[MAX_BUF_SIZE] = {0}, rddata[MAX_BUF_SIZE] = {0};
	unsigned short crc16;
	
	buf[len++] = SHA204_NORMAL;//cmd
	buf[len++] = MIN_PAKET_SIZE + MAC_CHALLENGE_SIZE;//len
	buf[len++] = SHA204_MAC;//op
	buf[len++] = mode;//0x20;//arg1
	buf[len++] = slot_id & 0xff;//0x00;//arg2 byte0
	buf[len++] = slot_id >> 8;//0x00;//arg2 byte1
	memcpy(&buf[len],challenge,MAC_CHALLENGE_SIZE);
	len += MAC_CHALLENGE_SIZE;
	
	sha204_calculate_crc(len - 1,&buf[1],&crc16);//must use this!
	memcpy(&buf[len],&crc16,PACKET_CHECKSUM_SIZE);
	len += PACKET_CHECKSUM_SIZE;

	ret = write_chip(fd,&buf,addr,len);
    //printf("write ret = %d\n",ret);
	if(ret != len + 1)
	{
		printf("write error ...\n");
		return -1;
	}
	//lib_printf_v2("write data:",&buf,ret - 1,16);
	lib_msleep(100);
#endif
#if 1//response
	memset(&rddata,0,MAX_BUF_SIZE);
	ret = read_chip(fd,&rddata,addr,MAC_RSP_SIZE);
	if(ret != MAC_RSP_SIZE)
	{
		printf("read error ...\n");
		return -2;
	}
    memcpy(rxbuf,&rddata,MAC_RSP_SIZE);
	lib_printf_v2("\nMAC Response:",&rddata,ret,16);
#endif

	lib_msleep(1);
	return 0;
}


/*计算校验码*/
void sha204_calculate_crc(unsigned char length, unsigned char *data, unsigned char *crc) 
{
	unsigned char counter;
	unsigned short crc_register = 0;
	unsigned short polynom = 0x8005;
	unsigned char shift_register;
	unsigned char data_bit, crc_bit;

	for (counter = 0; counter < length; counter++) {
	  for (shift_register = 0x01; shift_register > 0x00; shift_register <<= 1) {
		 data_bit = (data[counter] & shift_register) ? 1 : 0;
		 crc_bit = crc_register >> 15;

		 // Shift CRC to the left by 1.
		 crc_register <<= 1;

		 if ((data_bit ^ crc_bit) != 0)
			crc_register ^= polynom;
	  }
	}
	crc[0] = (unsigned char) (crc_register & 0x00FF);
	crc[1] = (unsigned char) (crc_register >> 8);
}


int fd = -1;
int LIB_Encry_Verify(void)
{
	int ret = -1, wake_up = 0;//唤醒标志 0为唤醒失败 1为成功
    //unsigned char random[RANDOM_NUM_SIZE] = {0};
	unsigned char mac_response[MAC_RSP_SIZE] = {0};
    
	
#if 0
	//expected MAC response in mode 0x20
	const unsigned char mac_response_expected[MAC_RSP_SIZE] =
	{
		MAC_RSP_SIZE,									// count
		0x3b, 0x0c, 0x14, 0x7c, 0x63, 0x07, 0x4a, 0x02, 
		0xca, 0x5b, 0x65, 0xa6, 0xae, 0x8b, 0x8a, 0xbe, 
		0x4c, 0xb5, 0xac, 0xaa, 0x1e, 0x4c, 0x87, 0x0d, 
		0x68, 0xea, 0x9c, 0x19, 0x71, 0x41, 0x82, 0x65,
		0x45,0x9b										// CRC
	};
#else
	//新板 新密钥
	const unsigned char mac_response_expected[MAC_RSP_SIZE] =
	{
		MAC_RSP_SIZE,									// count
		0x6a, 0xaf, 0x4b, 0x30, 0xbd, 0xfa, 0xd1, 0x8c,
		0xfb, 0xa2, 0x9d, 0xbe, 0xec, 0xc1, 0xf8, 0x19,
		0x9b, 0xce, 0x7e, 0xf3, 0x2b, 0x81, 0x61, 0xd5,
		0x22, 0x5b, 0x83, 0x27, 0xdb, 0x8f, 0x7e, 0xcd,
		0x12,0x8a										// CRC
	};
#endif
	

	//data for challenge in MAC mode 0x20 command
	const unsigned char challenge[MAC_CHALLENGE_SIZE] = {
		0x2d, 0xce, 0x04, 0x63, 0x36, 0x5c, 0x17, 0xad,
		0x02, 0xe8, 0x62, 0xc6, 0x20, 0x75, 0xc7, 0x2f, 
		0x5f, 0x4a, 0x98, 0x8f, 0x40, 0xe1, 0xe9, 0xda, 
		0x9f, 0x3d, 0x4b, 0x4d, 0x5e, 0x06, 0x10, 0x4f
	};

	
    //sha204初始化
    fd = i2c_dev_init((char *)CHIP, (char *)CHIP_ADDR);
	if(fd < 0)
    {
        close(fd);
		printf("\nsha204 init failed!\n");
		return LIB_EV_ERR;
    }   
	printf("\nsha204 init success!\n");

    //唤醒sha204
    ret = sha204_wakeup(fd);
    if(ret < 0)
    {
        printf("\nwakeup failed!\n");
        close(fd);
		return LIB_EV_ERR;
    }
	printf("\nwakeup success!\n");
	wake_up = 1;
	
#if 0
    //产生随机数
	ret = sha204_random(fd,&random,1);
    if(ret < 0)
    {
        //printf("\nRandom failed!\n");
        close(fd);
		return LIB_EV_ERR;
    } 
    //lib_printf_v2("\nRandom Num:",&random,RANDOM_NUM_SIZE,16);
#endif

#if 1
	//MAC 消息认证
	ret = sha204_mac(fd,(unsigned char *)&mac_response,0x20,0,(unsigned char *)&challenge);
	//ret = sha204_mac(fd,&mac_response,0x20,0,&random);
	if(ret < 0)
    {
        //printf("\nMAC error!\n");
        close(fd);
		return LIB_EV_ERR;
    } 
	//lib_printf_v2("\nSHA-256 digest:",&mac_response[1],MAC_RSP_DIGEST_SIZE,16);
	//printf("\n");

	if(memcmp(&mac_response_expected,&mac_response,MAC_RSP_SIZE) == 0)
	{
		printf("MAC Success!\n\n");
		goto exit;
	}else{
		printf("MAC failed!\n");
		close(fd);
		
		if(wake_up == 1)
			return LIB_WAKE_UP_SUCCESS;
		return LIB_EV_ERR;
	}
	//printf("Welcome to the System ...\n\n");
#endif


	//printf("Go into Sleep Mode ...\n\n");
	lib_sleep(2);//通过延时进入休眠(>2s)
	
exit:
	//close(fd); //fd为全局变量,后面读取设备信息后再close

	return LIB_EV_OK;
}

char *LIB_EV_Version(void)
{
      return EV_VER;
}


/*以下接口为临时测试用*/
/*zone 0: Configuration; 1: OTP; 2: Data
*address: byte address to read from*/
int sha204_read(int fd,unsigned char *rx_buf,unsigned char size,unsigned char zone,unsigned short address)
{
#if 1
	int len = 0, ret = -1, addr = 0;
	unsigned char buf[MAX_BUF_SIZE] = {0}, rddata[MAX_BUF_SIZE] = {0};
	unsigned short crc16;
	
	buf[len++] = SHA204_NORMAL;//cmd
	buf[len++] = MIN_PAKET_SIZE;//len
	buf[len++] = SHA204_READ;//op
	buf[len++] = zone;//arg1
	buf[len++] = (unsigned char)(address & SHA204_ADDRESS_MASK);//arg2 byte0
	buf[len++] = 0x00;//arg2 byte1

	sha204_calculate_crc(len - 1,&buf[1],&crc16);//must use this!
	memcpy(&buf[len],&crc16,PACKET_CHECKSUM_SIZE);
	len += PACKET_CHECKSUM_SIZE;

	ret = write_chip(fd,&buf,&addr,len);
    //printf("write ret = %d\n",ret);
	if(ret != len + 1)
	{
		printf("write error ...\n");
		return -1;
	}
	
	//lib_printf_v2("write data:",&buf,ret - 1,16);
	lib_msleep(100);
#endif
#if 1//response
	memset(&rddata,0,MAX_BUF_SIZE);
	ret = read_chip(fd,&rddata,addr,size);
	if(ret != size)
	{
		printf("read error ...\n");
		return -2;
	}

    memcpy(rx_buf,&rddata,ret);
	//lib_printf_v2("response:",&rddata,ret,16);
#endif

	return ret;
}

//读取节点机设备信息
int read_device_info(unsigned char *device_info)
{
	int ret = 0;
	unsigned char rx_buf[128] = {0};

	if(NULL == device_info)
	{
		return -1;
	}

	//sha204初始化
    fd = i2c_dev_init((char *)CHIP, (char *)CHIP_ADDR);
	if(fd < 0)
    {
		printf("\nsha204 init failed!\n");
		goto err;
    }   
	printf("\nsha204 init success!\n");

    //唤醒sha204
    ret = sha204_wakeup(fd);
    if(ret < 0)
    {
        printf("\nwakeup failed!\n");
		goto err;
    }
	printf("\nwakeup success!\n");
	
	//printf("fd:%d\n",fd);
    ret = sha204_read(fd,&rx_buf,35,SHA204_ZONE_DATA|0x80,0x00);//read data
    if(ret != 35)
    {
		printf("\nread device_info failed!\n");
		goto err;
	}
	printf("\nread device_info success!\n");
	memcpy(device_info,&rx_buf[1],32);

	close(fd);
	return ret - 3; //设备信息长度(32bytes)

err:
	close(fd);
	return LIB_EV_ERR;
}

int sha204_update_extra(int fd)
{
#if 1
	int len = 0, ret = -1, addr = 0;
	unsigned char buf[MAX_BUF_SIZE] = {0}, rddata[RANDOM_RSP_SIZE] = {0};
	unsigned short crc16;
	
	buf[len++] = 0x03;//cmd
	buf[len++] = 0x07;//len
	buf[len++] = 0x20;//op
	buf[len++] = 0x00;//arg1
	buf[len++] = 0x22;//arg2 byte0
	buf[len++] = 0x00;//arg2 byte1

	sha204_calculate_crc(len - 1,&buf[1],&crc16);//must use this!
	memcpy(&buf[len],&crc16,2);
	len += 2;

	ret = write_chip(fd,&buf[0],&addr,len);
	//ret = my_write_chip(fd,&buf[0],len);
	//printf("write ret = %d\n",ret);
	if(ret != len + 1)
	{
		printf("write error ...\n");
		return -1;
	}
	
	lib_printf_v2("write data:",&buf,ret - 1,16);
	lib_msleep(100);
#endif
#if 1//response
	memset(&rddata,0,RANDOM_RSP_SIZE);
	ret = read_chip(fd,&rddata,addr,RANDOM_RSP_SIZE);
	if(ret != RANDOM_RSP_SIZE)
	{
		printf("read error ...\n");
		return -2;
	}

    //memcpy(response,&rddata,RANDOM_RSP_SIZE);
	lib_printf_v2("response:",&rddata,ret,16);
#endif

	return 0;
}

int sha204_write(int fd)
{
#if 1
	int len = 0, ret = -1, addr = 0;
	unsigned char buf[MAX_BUF_SIZE] = {0}, wrdata[4] = {0xC2,0x42,0xAF,0x8F}, rddata[RANDOM_RSP_SIZE] = {0};
	unsigned short crc16;
	
	buf[len++] = 0x03;//cmd
	buf[len++] = 0x0b;//len
	buf[len++] = 0x12;//op
	buf[len++] = 0x00;//arg1
	buf[len++] = 0x0c;//arg2 byte0
	buf[len++] = 0x00;//arg2 byte1
	memcpy(&buf[len],&wrdata,4);
	len += 4;

	sha204_calculate_crc(len - 1,&buf[1],&crc16);//must use this!
	memcpy(&buf[len],&crc16,2);
	len += 2;

	ret = write_chip(fd,&buf[0],&addr,len);
	//printf("write ret = %d\n",ret);
	if(ret != len + 1)
	{
		printf("write error ...\n");
		return -1;
	}
	
	lib_printf_v2("write:",&buf,ret - 1,16);
	lib_msleep(100);
#endif
#if 1//response
	memset(&rddata,0,RANDOM_RSP_SIZE);
	ret = read_chip(fd,&rddata,addr,RANDOM_RSP_SIZE);
	if(ret != RANDOM_RSP_SIZE)
	{
		printf("read error ...\n");
		return -2;
	}

    //memcpy(response,&rddata,RANDOM_RSP_SIZE);
	lib_printf_v2("response:",&rddata,ret,16);
#endif

	return 0;
}

int sha204_nonce(int fd,unsigned char Mode,unsigned char *NumIn)
{
#if 1
	int len = 0, ret = -1, addr = 0;
	unsigned char buf[256] = {0}, rddata[256] = {0};
	unsigned short crc16;
	
	buf[len++] = 0x03;//cmd
	buf[len++] = 0x07;//len
	buf[len++] = 0x16;//op
	buf[len++] = 0x02;//arg1
	buf[len++] = 0x00;//arg2 byte0
	buf[len++] = 0x00;//arg2 byte1
	
	sha204_calculate_crc(len - 1,&buf[1],&crc16);//must use this!
	memcpy(&buf[len],&crc16,2);
	len += 2;

	ret = write_chip(fd,&buf[0],&addr,len);
	//printf("write ret = %d\n",ret);
	if(ret != len + 1)
	{
		printf("write error ...\n");
		return -1;
	}
	
	lib_printf_v2("write data:",&buf,ret - 1,16);
	lib_msleep(100);
#endif
#if 1//response
	memset(&rddata,0,MAX_BUF_SIZE);
	ret = read_chip(fd,&rddata,addr,RANDOM_RSP_SIZE);
	if(ret != RANDOM_RSP_SIZE)
	{
		printf("read error ...\n");
		return -2;
	}
    //memcpy(rxbuf,&rddata[1],32);
	lib_printf_v2("\nResponse:",&rddata,ret,16);
#endif
	
	return 0;
}



int sha204_command_test(int fd,unsigned char *rxbuf)
{
#if 1
	int len = 0, ret = -1, addr = 0;
	unsigned char buf[256] = {0}, rddata[256] = {0};
	unsigned short crc16;
	
	buf[len++] = 0x03;//cmd
	buf[len++] = 0x07;//len
	buf[len++] = 0x15;//op
	buf[len++] = 0x01;//arg1
	buf[len++] = 0x00;//arg2 byte0
	buf[len++] = 0x00;//arg2 byte1
	
	sha204_calculate_crc(len - 1,&buf[1],&crc16);//must use this!
	memcpy(&buf[len],&crc16,2);
	len += 2;

	ret = write_chip(fd,&buf[0],&addr,len);
	//printf("write ret = %d\n",ret);
	if(ret != len + 1)
	{
		printf("write error ...\n");
		return -1;
	}
	
	lib_printf_v2("write data:",&buf,ret - 1,16);
	lib_msleep(100);
#endif
#if 1//response
	memset(&rddata,0,MAX_BUF_SIZE);
	ret = read_chip(fd,&rddata,addr,RANDOM_RSP_SIZE);
	if(ret != RANDOM_RSP_SIZE)
	{
		printf("read error ...\n");
		return -2;
	}
    memcpy(rxbuf,&rddata[1],32);
	lib_printf_v2("\nResponse:",&rddata,ret,16);
#endif
	
	return 0;
}

