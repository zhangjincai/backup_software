#include "include/lib_general.h"
#include "include/lib4g.h"
#include <unistd.h>

//#define DEV_NAME "/dev/ttyUSB3"
#define DEV_NAME "/dev/MC27161"


int main(int argc, char **argv)
{
	lib_serial_t *serial;
	struct tm my_4g_tm;
	gps_attr_t my_4g_attr;
	lib_4g_gga_t my_4g_gga;
	
	int temp=0;
	int total=0;
	int disconnect=0;
	char rxbuf[1024];
	char txbuf[1024];
	int __ret = -1;
	int card_type;
	int card_status;
	int card_decode_status;

	serial = (lib_serial_t *)malloc(sizeof(lib_serial_t));
	strcpy(serial->pathname, DEV_NAME);
	serial->flags = O_RDWR;
	serial->speed = 115200;
	serial->databits = 8;
	serial->stopbits = 1;

	__ret = lib_serial_init(serial);
	if(__ret<0)
	{
		printf("lib_serial_init error!\n");
		return -1;
	}

	__ret = lib_4g_ate0(serial->sfd);
	if(__ret<0)
	{
		printf("lib_4g_ate0 error!\n");
	}

	// 查看SIM卡状态
	__ret = lib_4g_check_cpin(serial->sfd);
	card_status=0;
	card_decode_status = false;
	if(__ret<0)
	{
		printf("lib_4g_check_cpin error!\n");
	}else{
		card_status = __ret;
		switch(card_status)
		{
		case CARD_STATUS_READY: {printf("CARD_STATUS_READY\n"); break;}
		case CARD_STATUS_SIM_PIN: {printf("CARD_STATUS_SIM_PIN\n"); card_decode_status = true; break;}
		case CARD_STATUS_NO_INSERTED: {printf("CARD_STATUS_NO_INSERTED\n"); return -1; break;}
		case CARD_STATUS_NO_INSERTED_AND_ERROR: {printf("CARD_STATUS_NO_INSERTED_AND_ERROR\n");return -1; break;}
		default: {return -1; break;}
		}
	}	

	
	//	本人移动卡加密
	if(card_decode_status == true)
	{
		lib_4g_decode(serial->sfd,"0609");
		card_decode_status = false;

		// 查看SIM卡状态
		__ret = lib_4g_check_cpin(serial->sfd);
		card_status=0;
		if(__ret<0)
		{
			printf("lib_4g_check_cpin error!\n");
		}else{
			card_status = __ret;
			switch(card_status)
			{
			case CARD_STATUS_READY: {printf("CARD_STATUS_READY\n"); break;}
			case CARD_STATUS_SIM_PIN: {printf("CARD_STATUS_SIM_PIN\n"); card_decode_status = true; break;}
			case CARD_STATUS_NO_INSERTED: {printf("CARD_STATUS_NO_INSERTED\n"); return -1; break;}
			case CARD_STATUS_NO_INSERTED_AND_ERROR: {printf("CARD_STATUS_NO_INSERTED_AND_ERROR\n");return -1; break;}
			default: {return -1; break;}
			}
		}	
	}

	// 查看信号强度
	int rssi;
	rssi = lib_4g_get_csq(serial->sfd);
	if(rssi<0)
	{
		printf("lib_4g_ate0 error!\n");
	}else
	{
		printf("the signal strength is %d\n",rssi);
	}


	//网络查看sim卡为什么卡
	int i;
	//for(i=0;i<100;i++)
	{
		__ret = lib_4g_check_cops(serial->sfd);

	 	printf("lib_4g_check_cops! ret = %d  i = %d\n",__ret,i);
		
		card_type=0;
		if(__ret<0)
		{
			printf("lib_4g_check_cops error!  ret = %d  i = %d\n",__ret,i);
		}else{
			card_type = __ret;
			switch(card_type)
			{
			case CARD_CMCC:printf("the card is CMCC\n"); i=10; break;
			case CARD_CUCC:printf("the card is CUCC\n"); i=10; break;
			case CARD_CTCC:printf("the card is CTCC\n"); i=10; break;
			default:break;
			}
		}
	}
	


#if 0
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+CRSM = 176,12258,0,0,10\r\n");	
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(serial->sfd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(serial->sfd, rxbuf, 1024, 2000);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("%s\n",rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
	}
#endif	


ECM_4G_UNCONNECT:
#if 1
	// 拨号  移动卡
	if(card_type == CARD_CMCC)
	{
		__ret = lib_4g_CMCC_dialing(serial->sfd);
		if(__ret<0)
		{
			printf("lib_4g_CMCC_dialing error!\n");
		}
	}else if((card_type == CARD_CUCC) || (card_type == CARD_CTCC))    // 联通卡和电信卡
	{
		__ret = lib_4g_CUCC_CTCC_dialing(serial->sfd);
		if(__ret<0)
		{
			printf("lib_4g_CUCC_CTCC_dialing error!\n");
		}
	}else{
		printf("card_type hasn`t type!\n");
	}
		
	
	
	
#endif


#if 0
	// 断开连接
	__ret = lib_4g_ecm_disconnect(serial->sfd);
	if(__ret<0)
	{
		printf("lib_4g_ecm_disconnect error!\n");
	}
#endif


#if 0
		// 检测连接
		__ret = lib_4g_ecm_connect_status(serial->sfd);
		if(__ret==ECM_4G_SERIAL_ERROR)
		{
			printf("lib_4g_ecm_connect_status error!\n");
			goto ECM_4G_UNCONNECT;
		}else if(__ret==ECM_4G_UNCONNECT)
			printf("lib_4g_ecm_connect_status disconnect!\n");
		else
			printf("lib_4g_ecm_connect_status connect!\n");
		
#endif


#if 0

//printf_warning("GPS");
	// 获取GPS数据
	while(1)
	{
		total++;
		memset(rxbuf,0,1024);
		__ret = lib_4g_get_gps_info(serial->sfd,rxbuf,30000);
		if(__ret<0)
		{
			
			printf("lib_4g_get_gps_info error!\n");
			temp++;
		}else
		{
			memset(&my_4g_tm,0,sizeof(struct tm));
			memset(&my_4g_attr,0,sizeof(gps_attr_t));
			memset(&my_4g_gga,0,sizeof(lib_4g_gga_t));
			
			__ret = lib_4g_analytic_zgpsr(rxbuf, &my_4g_tm, &my_4g_gga, &my_4g_attr);
			if(__ret!=-1)
			{
				printf("gps_show_tm ****************************************\n");
				gps_show_tm(&my_4g_tm);
				printf("gps_show_gga ****************************************\n");
				gps_show_gga(&my_4g_gga);
			}
		}

		// 检测连接
		__ret = lib_4g_ecm_connect_status(serial->sfd);
		if(__ret==ECM_4G_SERIAL_ERROR)
		{
			printf("lib_4g_ecm_connect_status error!\n");
			disconnect++;
			goto ECM_4G_UNCONNECT;
		}else if(__ret==ECM_4G_UNCONNECT)
			{
			printf("lib_4g_ecm_connect_status disconnect!\n");
			disconnect++;
			goto ECM_4G_UNCONNECT;
		}else{
			printf("lib_4g_ecm_connect_status connect!\n");
		}
	}


		printf("total = %d \\\\\\\\\\\\\\\\\\\\\\\\\\\\ getErrror = %d \\\\\\\\\\\\\\\\\\\\\\\\\\\\ disconnect =%d \n",total,temp,disconnect);

	}
#endif

	lib_serial_close(serial);

	return 0;
}
