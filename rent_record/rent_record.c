#include <stdio.h>
#include "lib_rent_record.h"
#include <stdlib.h>
#include <string.h>


int ParseRentBikeRecord(char* buff,int len)
{
	if(buff == NULL || len <= 0)
	{	
		return -1;
	}
	
	int begin = 11;//从第12个字节开始为租用记录内容
	int end = 0;	
	int cnt = 0;
	int size = 0;
	int i = 0;
	int m_CurrRecordNum = 0;
	
	rent_info_fmt_t *m_RentRecordInfo = NULL;
	m_RentRecordInfo = calloc(buff[8],sizeof(rent_info_fmt_t));//根据记录条数来申请空间

	for(i = 11; i < len + 11; i++)//从第12个字节开始为租用记录内容
	//for(i = 0; i < len; i++)//从第12个字节开始为租用记录内容
	{
		//0x09:字段间的分隔符(tab键), 0x10:记录间的分隔符(换行键)
		if((buff[i] == 0x09) || (buff[i] == 0x0a))
		{
			end = i;
			size = end - begin;//一个字段的长度
			switch(cnt)//将各个字段拷到结构体对应的成员中去
			{
				case 0://自行车编号
					if(size >= 32)
					{
						size = 32;
					}
					if(m_CurrRecordNum == 0)
						memcpy(m_RentRecordInfo[m_CurrRecordNum].bike_NO,\
					   		&buff[begin],size);
					else
						memcpy(m_RentRecordInfo[m_CurrRecordNum].bike_NO,\
					   		&buff[begin+1],size);
					
					m_RentRecordInfo[m_CurrRecordNum].bike_NO[size] = '\0';
					break;
				case 1://租车点名称
					if(size >= 64)
					{
						size = 64;
					}
					memcpy(m_RentRecordInfo[m_CurrRecordNum].rent_name,\
						   &buff[begin+1],size);
					m_RentRecordInfo[m_CurrRecordNum].rent_name[size] = '\0';
					break;
				case 2://租车点时间
					if(size >= 32)
					{
						size = 32;
					}
					memcpy(m_RentRecordInfo[m_CurrRecordNum].rent_time,\
						   &buff[begin+1],size);
					m_RentRecordInfo[m_CurrRecordNum].rent_time[size] = '\0';
					break;
				case 3://还车点名称
					if(size >= 64)
					{
						size = 64;
					}
					memcpy(m_RentRecordInfo[m_CurrRecordNum].return_name,\
				   			&buff[begin+1],size);
					m_RentRecordInfo[m_CurrRecordNum].return_name[size] = '\0';					
					break;
				case 4://还车时间
					if(size >= 32)
					{
						size = 32;
					}
					memcpy(m_RentRecordInfo[m_CurrRecordNum].return_time,\
		   					&buff[begin+1],size);
					m_RentRecordInfo[m_CurrRecordNum].return_time[size] = '\0';
					break;
				case 5://骑行时间
					if(size >= 16)
					{
						size = 16;
					}
					memcpy(&m_RentRecordInfo[m_CurrRecordNum].used_time,\
				   			&buff[begin+1],size);
					m_RentRecordInfo[m_CurrRecordNum].used_time[size] = '\0';
					break;
				case 6://扣费
					if(size >= 16)
					{
						size = 16;
					}
					memcpy(m_RentRecordInfo[m_CurrRecordNum].fee,\
				   			&buff[begin+1],size);
					m_RentRecordInfo[m_CurrRecordNum].fee[size] = '\0';
					break;
				default:
					cnt = 0;
					break;
			}
			begin = end;
			cnt++;//同一条记录里的第几个字段
		}	
		if(buff[i] == 0x0a)
		{	
			//打印显示每一条租用记录
			printf("---------RecordNum:[%d]---------\n",m_CurrRecordNum);
			printf("bike_NO:%s\n",m_RentRecordInfo[m_CurrRecordNum].bike_NO);
			printf("rent_name:%s\n",m_RentRecordInfo[m_CurrRecordNum].rent_name);
			printf("rent_time:%s\n",m_RentRecordInfo[m_CurrRecordNum].rent_time);
			printf("return_name:%s\n",m_RentRecordInfo[m_CurrRecordNum].return_name);
			printf("return_time:%s\n",m_RentRecordInfo[m_CurrRecordNum].return_time);
			printf("used_time:%s\n",m_RentRecordInfo[m_CurrRecordNum].used_time);
			printf("fee:%s\n",m_RentRecordInfo[m_CurrRecordNum].fee);

			m_CurrRecordNum++;//第几条记录
			cnt = 0;
		}
	}

	if(m_RentRecordInfo != NULL)
	{
		free(m_RentRecordInfo);
		m_RentRecordInfo = NULL;
	}
	
	return 0;
}


