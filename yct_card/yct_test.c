
#include "lib_general.h"
#include "yct_card.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define	DEVNAME	"/dev/ttyO1"

lib_serial_t uart1;

int main(int argc,char *argv[])
{
	int ret = -1;
	char *pData,*szVer,*RentData,*pLen;
	char tbuf[128] = {0},rbuf[128] = {0};
	char pSPno[2] = {0}, pSamID[4] = {0};
	//lib_serial_t uart1;
  	memset(&uart1, 0, sizeof(lib_serial_t));
  	//串口初始化
  	strcpy(uart1.pathname,DEVNAME);
  	uart1.flags = O_RDWR;
  	uart1.speed = 115200;
  	uart1.databits = 8;
  	uart1.stopbits = 1;
		 
 	ret = lib_serial_init(&uart1);
	printf("uart init ret:%d,sfd:%d\n",ret, uart1.sfd);
	///////////////////////////////////////////
	ret = Module_Init(pSPno,pSamID);//核心模块初始化
	GetVersion(szVer);//获取程序版本
	QuerTicketInfo(pData);//查询票卡信息
	//QuerRentInfo(0x38,RentData,pLen);//查询租还车信息
	
	return 0;
}

/////////////////////////////////////////////////
int GetVersion(char* szVer)//获取程序版本
{
	int ret = -1;
	char tbuf[4] = {0},rbuf[128] = {0};
	
	tbuf[0] = 0xBA;//header
	tbuf[1] = 0x02;//len
	tbuf[2] = 0x60;//cmd
	tbuf[3] = CheckSum(&tbuf[0], 3);//checksum

	ret = lib_serial_send(uart1.sfd, &tbuf, sizeof(tbuf));
	lib_vprintf("send %d bytes\n",ret);
	lib_printf("------send------\n", tbuf, ret);
	lib_msleep(300);

	ret = lib_serial_recv(uart1.sfd, &rbuf, sizeof(rbuf));
	memcpy(szVer,&rbuf[4],24);
	//lib_printf("------recv------\n", rbuf, ret);
	lib_vprintf("software version:%s\n",szVer);
	
	return 0;
}
//核心模块初始化
//输入pSPno ： 服务商代码 输出pSamID：PSAM 卡号
int Module_Init(char pSPno[2], char pSamID[4])
{
	int ret = -1;
	char tbuf[6] = {0},rbuf[128] = {0};
	
	tbuf[0] = 0xBA;//header
	tbuf[1] = 0x04;//len
	tbuf[2] = 0xD2;//cmd
	memcpy(&tbuf[3],pSPno,2);//data 0
	tbuf[5] = CheckSum(&tbuf[0], 7);//checksum

	ret = lib_serial_send(uart1.sfd, &tbuf, sizeof(tbuf));
	lib_vprintf("send %d bytes\n",ret);
	lib_printf("------send------\n", tbuf, 6);
	lib_msleep(300);

	ret = lib_serial_recv(uart1.sfd, &rbuf, sizeof(rbuf));
	memcpy(pSamID,&rbuf[4],4);
	//lib_printf("------recv------\n", rbuf, ret);
	
	return 0;
}

//查询票卡信息
int QuerTicketInfo(char* pData)
{
	int ret = -1;
	char tbuf[4] = {0},rbuf[128] = {0};
	
	tbuf[0] = 0xBA;//header
	tbuf[1] = 0x02;//len
	tbuf[2] = 0xD3;//cmd
	tbuf[3] = CheckSum(&tbuf[0], 3);//checksum

	ret = lib_serial_send(uart1.sfd, &tbuf, sizeof(tbuf));
	lib_vprintf("send %d bytes\n",ret);
	lib_printf("------send------\n", tbuf, 4);
	lib_msleep(300);

	ret = lib_serial_recv(uart1.sfd, &rbuf, sizeof(rbuf));
	memcpy(&pData,&rbuf[4],1);
	lib_printf("------recv------\n", rbuf, ret);

	return 0;
}
//查询租还车信息
int QuerRentInfo(char bID,char* pData,char* pLen)
{
	int ret = -1;
	char tbuf[5] = {0},rbuf[128] = {0};
	
	tbuf[0] = 0xBA;//header
	tbuf[1] = 0x03;//len
	tbuf[2] = 0xD9;//cmd
	memcpy(&tbuf[3],&bID,1);//data 
	tbuf[4] = CheckSum(&tbuf[0], 4);//checksum

	ret = lib_serial_send(uart1.sfd, &tbuf, sizeof(tbuf));
	lib_vprintf("send %d bytes\n",ret);
	lib_printf("------send------\n", tbuf, ret);
	lib_msleep(300);

	ret = lib_serial_recv(uart1.sfd, &rbuf, sizeof(rbuf));
	memcpy(&pData,&rbuf[4],1);
	lib_printf("------recv------\n", rbuf, ret);
	
	return 0;
}
//设置租还车信息
//输入： bID ： 复合应用类型标识符（自行车一般应用（ 0x38），增值会员应用（ 0x68））
//pData ： 待设置的租还车信息
int SetRentInfo(char bID,char* pData,char* pLen)
{
	int ret = -1;
	char tbuf[3] = {0},rbuf[128] = {0};
	
	tbuf[0] = 0xBA;//header
	tbuf[1] = 0x04;//len
	tbuf[2] = 0xDA;//cmd
	memcpy(&tbuf[3],&bID,1);
	memcpy(&tbuf[4],pData,1);
	tbuf[5] = CheckSum(&tbuf[0], 5);//checksum

	ret = lib_serial_send(uart1.sfd, &tbuf, sizeof(tbuf));
	lib_vprintf("send %d bytes\n",ret);
	lib_msleep(300);

	ret = lib_serial_recv(uart1.sfd, &rbuf, sizeof(rbuf));
	pLen = (char *)ret;
	lib_printf("------recv------\n", rbuf, ret);
	
	return 0;
}
//消费
int Consume(long lAmount, long lFee, char pTime[6], char bPHysicalID[8], char bAddtionalType)
{
	int ret = -1;
	char tbuf[27] = {0},rbuf[128] = {0};
	
	tbuf[0] = 0xBA;//header
	tbuf[1] = 0x19;//len 25
	tbuf[2] = 0xD4;//cmd
	memcpy(&tbuf[3],&lAmount,4);
	memcpy(&tbuf[7],&lFee,4);
	memcpy(&tbuf[11],pTime,6);
	memcpy(&tbuf[17],bPHysicalID,8);
	memcpy(&tbuf[25],&bAddtionalType,1);
	tbuf[26] = CheckSum(&tbuf[0], 26);//checksum

	ret = lib_serial_send(uart1.sfd, &tbuf, sizeof(tbuf));
	lib_vprintf("send %d bytes\n",ret);
	lib_msleep(300);

	ret = lib_serial_recv(uart1.sfd, &rbuf, sizeof(rbuf));
	lib_printf("------recv------\n", rbuf, ret);
	
	return 0;
}
//设置黑名单
int SetBlackFlag(char pData[16])
{
	int ret = -1;
	char tbuf[4] = {0},rbuf[128] = {0};
	
	tbuf[0] = 0xBA;//header
	tbuf[1] = 0x02;//len
	tbuf[2] = 0xD5;//cmd
	tbuf[3] = CheckSum(&tbuf[0], 3);//checksum

	ret = lib_serial_send(uart1.sfd, &tbuf, sizeof(tbuf));
	lib_vprintf("send %d bytes\n",ret);
	lib_msleep(300);

	ret = lib_serial_recv(uart1.sfd, &rbuf, sizeof(rbuf));
	lib_printf("------recv------\n", rbuf, ret);
	
	return 0;
}
//查询最近历史交易记录
int GetHisRecord(char* pType, char* pData)
{
	int ret = -1;
	char tbuf[4] = {0},rbuf[128] = {0};
	
	tbuf[0] = 0xBA;//header
	tbuf[1] = 0x02;//len
	tbuf[2] = 0xD6;//cmd
	tbuf[3] = CheckSum(&tbuf[0], 3);//checksum

	ret = lib_serial_send(uart1.sfd, &tbuf, sizeof(tbuf));
	lib_vprintf("send %d bytes\n",ret);
	lib_msleep(300);

	ret = lib_serial_recv(uart1.sfd, &rbuf, sizeof(rbuf));
	memcpy(pType, &rbuf[4], 1);
	memcpy(pData,&rbuf[5],ret -6);
	lib_printf("------recv------\n", rbuf, ret);

	return 0;
}
//获取交易数据
int GetTrans(char bTrans[90])
{
	int ret = -1;
	char tbuf[4] = {0},rbuf[128] = {0};
	
	tbuf[0] = 0xBA;//header
	tbuf[1] = 0x02;//len
	tbuf[2] = 0xD8;//cmd
	tbuf[3] = CheckSum(&tbuf[0], 3);//checksum

	ret = lib_serial_send(uart1.sfd, &tbuf, sizeof(tbuf));
	lib_vprintf("send %d bytes\n",ret);
	lib_msleep(300);

	ret = lib_serial_recv(uart1.sfd, &rbuf, sizeof(rbuf));
	memcpy(bTrans,&rbuf[4],90);
	lib_printf("------recv------\n", rbuf, ret);

	return 0;
}
//参数设置
int Set_parameter(char arg[3])
{
	int ret = -1;
	char tbuf[4] = {0},rbuf[5] = {0};
	
	tbuf[0] = 0xBA;//header
	tbuf[1] = 0x05;//len
	tbuf[2] = 0xD7;//cmd
	tbuf[3] = CheckSum(&tbuf[0], 6);//checksum

	ret = lib_serial_send(uart1.sfd, &tbuf, sizeof(tbuf));
	lib_vprintf("send %d bytes\n",ret);
	lib_msleep(300);

	ret = lib_serial_recv(uart1.sfd, &rbuf, sizeof(rbuf));
	lib_printf("------recv------\n", rbuf, ret);

	return 0;
}
//校验码
unsigned char CheckSum(unsigned char *data,const unsigned char len)
{
	int i;
	unsigned char check;
	for(i = 0; i < len; i++)
	{
		check ^= data[i];
	}

	return check;
}
















