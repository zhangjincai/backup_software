
#ifndef __YCT_CARD_H_
#define __YCT_CARD_H_

int GetVersion(char* szVer);//获取程序版本
int Module_Init(char pSPno[2], char pSamID[4]);//核心模块初始化
int QuerTicketInfo(char* pData);//查询票卡信息
int QuerRentInfo(char bID,char* pData,char* pLen);//查询租还车信息
int SetRentInfo(char bID,char* pData,char* pLen);//设置租还车信息
int Consume(long lAmount, long lFee, char pTime[6], char bPHysicalID[8], char bAddtionalType);//消费
int SetBlackFlag(char pData[16]);//设置黑名单
int GetHisRecord(char* pType, char* pData);//最近交易记录
int GetTrans(char bTrans[90]);//获取交易数据
unsigned char CheckSum(unsigned char *data,const unsigned char len);//校验码

#endif // __yct_card_H__
