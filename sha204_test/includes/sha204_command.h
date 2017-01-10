#ifndef __SHA204_COMMAND_H__
#define __SHA204_COMMAND_H__

#define CHIP			"/dev/i2c-3"    //i2c总线
#define CHIP_ADDR		0x64    		//i2c设备地址

#define EV_VER       "Encry Verify Version 1.0.1"


#define	RANDOM_RSP_SIZE 35
#define	RANDOM_NUM_SIZE 32
#define	MAC_RSP_SIZE    35
#define	WAKEUP_RSP_SIZE 4 

#define MAX_BUF_SIZE        128
#define MIN_PAKET_SIZE      7

#define MAC_CHALLENGE_SIZE      32
#define MAC_RSP_DIGEST_SIZE     32
#define PACKET_CHECKSUM_SIZE    2

//command op-code definitions
#define SHA204_RANDOM       0x1B
#define SHA204_MAC          0x08
#define SHA204_READ         0x02
#define SHA204_UPDATE_EXTRA 0x20
#define SHA204_WRITE        0x12

#define SHA204_ADDRESS_MASK 0x007F   //Address bit 7 to 15 are always 0.

//zone definitions
#define SHA204_ZONE_CONFIG              (0x00)      //!< Configuration zone
#define SHA204_ZONE_OTP                 (0x01)      //!< OTP (One Time Programming) zone
#define SHA204_ZONE_DATA                (0x02)      //!< Data zone

/** \brief This enumeration lists all packet types sent to a SHA204 device.
 *
 * The following byte stream is sent to a SHA204 TWI device:
 *    {I2C start} {I2C address} {word address} [{data}] {I2C stop}.
 * Data are only sent after a word address of value #SHA204_I2C_PACKET_FUNCTION_NORMAL.
 */
enum command_flag {
	SHA204_RESET,  //!< Reset device.
	SHA204_SLEEP,  //!< Put device into Sleep mode.
	SHA204_IDLE,   //!< Put device into Idle mode.
	SHA204_NORMAL  //!< Write / evaluate data that follow this word address byte.
};

/*sha204操作接口*/
int sha204_wakeup(int fd);
int sha204_sleep(int fd);
int sha204_random(int fd,unsigned char *rxbuf,unsigned char mode);
int sha204_mac(int fd,unsigned char *rxbuf,unsigned char mode,unsigned short slot_id,unsigned char *challenge);
int sha204_read(int fd,unsigned char *rx_buf,unsigned char size,unsigned char zone,unsigned short address);
//int read_device_info(int fd, unsigned char *rx_buf);

int sha204_write(int fd);
int sha204_update_extra(int fd);
int sha204_nonce(int fd,unsigned char Mode,unsigned char *NumIn);
int sha204_command_test(int fd,unsigned char *rxbuf);

void sha204_calculate_crc(unsigned char length, unsigned char *data, unsigned char *crc);



#endif/*#ifndef __SHA204_COMMAND_H__*/
