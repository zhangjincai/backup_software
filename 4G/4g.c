#include "include/lib4g.h"

char txbuf[1024];
char rxbuf[1024];
char buf[1024];
int __ret;
char*p;


int lib_4g_ate0(int fd)
{
	// 关闭回显
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"ATE0\r\n");	
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("%s\n",rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	return 0;
}
	
int lib_4g_ecm_disconnect(int fd)
{
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZECMCALL=0\r\n");	
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("%s\n",rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	return 0;
}

int lib_4g_ecm_connect(int fd)
{
	// ECM DATA CALL
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZECMCALL=1\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 8000);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
		if(!strcmp(rxbuf,"\r\n+CME ERROR: unknown\r\n"))
			return -1;
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}

	system("udhcpc -i usb0");

	return 0;
}

int lib_4g_ecm_connect_status(int fd)
{
	// ECM DATA CALL
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZECMCALL?\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return ECM_4G_SERIAL_ERROR;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return ECM_4G_SERIAL_ERROR;
	}

	if(__ret>20)
		return ECM_4G_CONNECT;

	return ECM_4G_UNCONNECT;
}



int lib_4g_decode(int fd, char* key)
{
	// CPIN 密码puk
	printf("************************************\n");
	char command[30];
	strcpy(command,"AT+CPIN=\"");
	strcat(command,key);
	strcat(command,"\"\r\n");
		printf("command : %s\n",command);
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+CPIN=\"0609\"\r\n");
	//strcpy(txbuf,command);	
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 2000);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("%s\n",rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	return 0;
}


int lib_4g_restart(int fd)
{
	// 重启
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+CFUN=1,1\r\n");	
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("%s\n",rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	return 0;
}

int lib_4g_setECMforWindows_and_restart(int fd)
{
	// 设置状态
	// 选择ECM模式
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZSWITCH=X\r\n");	
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}
	
	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("%s\n",rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	lib_4g_restart(fd);

	return 0;
}

int lib_4g_setECMforLinux_and_restart(int fd)
{
	// 设置状态
	// 选择ECM模式
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZSWITCH=L\r\n");	
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}
	
	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("%s\n",rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	lib_4g_restart(fd);

	return 0;
}

int lib_4g_get_csq(int fd)
{
	// 信号强度
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+CSQ\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}
	

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		strcpy(buf,rxbuf);

		p=strtok(buf," ");	
		if(p)
		{
        	p=strtok(NULL,",");
		}
		
		return atoi(p);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
}

int lib_4g_check_cpin(int fd)
{
	// SIM识别
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+CPIN?\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}
	
	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		printf("len = %d, %s\n",__ret,rxbuf);
		if(!strcmp(rxbuf,"\r\n+CPIN: READY\r\n\r\nOK\r\n"))
		{
			return CARD_STATUS_READY;
		}else if(!strcmp(rxbuf,"\r\n+CPIN: SIM PIN\r\n\r\nOK\r\n"))
		{
			return CARD_STATUS_SIM_PIN;
		}else if(!strcmp(rxbuf,"\r\n+CME ERROR: SIM not inserted\r\n"))
		{
			return CARD_STATUS_NO_INSERTED;
		}else if(!strcmp(rxbuf,"\r\n+CME ERROR: 10\r\n"))
		{
			return CARD_STATUS_NO_INSERTED_AND_ERROR;
		}
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}

	return 0;
}

// 检查card信息  移动CMCC 联通chn-unicom 电信????
int lib_4g_check_cops(int fd)
{
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+COPS?\r\n");	
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
	}

	memset(rxbuf,0,sizeof(rxbuf));

	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		printf("ret = %d ,%s\n",__ret,rxbuf);
		if(__ret<20)
		{
			return __ret;
		}
		strcpy(buf,rxbuf);
		p=strtok(buf,"\"");	
		if(p)
		{
        	p=strtok(NULL,"\"");
		}
		if(!strcmp(p,"CMCC"))
		{
			return CARD_CMCC;
		}else if(!strcmp(p,"????"))
		{
			return CARD_CTCC;
		}else if(!strcmp(p,"CHN-UNICOM"))
		{
			return CARD_CUCC;
		}
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
	}

	return 0;
}

int lib_4g_zpas(int fd)
{
// ZPAS 查看上网的方式2,3,4G 
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZPAS?\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
}


int lib_4g_get_basic_info(int fd)
{
	// CEREG  查询网络状态
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+CEREG?\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}
	

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
		

	
	
	

	// AT+CGMR   软件版本
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+CGMR\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}


	// ATI
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"ATI\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	return 0;
}

// 屏蔽2G 3G信号
int lib_4g_zsnt(int fd)
{
	// AT+ZSNT=6,0,0   屏蔽2G 3G信号
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZSNT=6,0,0\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
}


// 移动拨号上网
int lib_4g_CMCC_dialing(int fd)
{
	__ret = lib_4g_zsnt(fd);
	if(__ret<0)
	{
		return -1;
	}
	
	// 选择ECM参数   ,\"CMNET\"中国移动    AT+CGDCONT=1,"IP"
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("%s\n",rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	__ret = lib_4g_ecm_connect(fd);
	if(__ret<0)
	{
		return -1;
	}

	return 0;
}

// 联通、电信拨号上网
int lib_4g_CUCC_CTCC_dialing(int fd)
{
	__ret = lib_4g_zsnt(fd);
	if(__ret<0)
	{
		return -1;
	}
	
	// 选择ECM参数   ,\"CMNET\"中国移动    AT+CGDCONT=1,"IP"
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+CGDCONT=1,\"IP\"\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("%s\n",rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	__ret = lib_4g_ecm_connect(fd);
	if(__ret<0)
	{
		return -1;
	}

	return 0;
}



// GPS获取
int lib_4g_getGPS_init(int fd)
{
	// GPS INIT
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZGINIT\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	// GPS ZGFIXRATE
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZGFIXRATE=2,5\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	// GPS ZGQOS
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZGQOS=50,255\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	// GPS ZGPSR
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZGPSR=1\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	// GPS ZGMODE
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZGMODE=3\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
/*
	// GPS ZGRUN 执行一次
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZGRUN=1\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	// 接收数据
	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 30000);
	if(__ret>0)
	{
		//lib_printf("rxbuf-------------------",rxbuf,__ret);
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
*/	
	return 0;
}

int lib_4g_get_gps_info(int fd, char *buf, int msec)
{
	// GPS ZGRUN 执行一次
	printf("************************************\n");
	memset(txbuf,0,sizeof(txbuf));
	strcpy(txbuf,"AT+ZGRUN=1\r\n");
	printf("txbuf size = %d  : %s\n",strlen(txbuf),txbuf);

	__ret = lib_serial_send(fd, txbuf, strlen(txbuf));
	if(__ret<0)
	{
		printf("lib_serial_send error!\n");
		return __ret;
	}

	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, 500);
	if(__ret>0)
	{
		printf("len = %d, %s\n",__ret,rxbuf);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}
	
	// 接收数据
	memset(rxbuf,0,sizeof(rxbuf));
	
	__ret = lib_serial_readn_select(fd, rxbuf, 1024, msec);
	if(__ret>0)
	{
		//printf("len = %d, %s\n",__ret,rxbuf);
		memcpy(buf,rxbuf,__ret);
	}else
	{
		fprintf(stderr, "__ret = %d\n", __ret);
		return __ret;
	}

	return 0;
}

int lib_4g_analytic_zgpsr(char *rxbuf, struct tm *my_4g_tm, lib_4g_gga_t *my_4g_gga, gps_attr_t *attr)
{
	printf("rxbuf: %s\n",rxbuf);

	char *q = "456.789";
	int index;
	double temp; 
	char *p;

	p = strtok(rxbuf,"\n");


	// +ZGPSR:
	p = strtok(NULL," ");
	if(p){
		printf("+ZGPSR:: %s\n",p);
		if(strcmp(p,"+ZGPSR:"))
		{
			fprintf(stderr,"ananlytic isn`t zgpsr!\n");
			my_4g_gga->status = 0;
			return -1;
		}
		my_4g_gga->status = 1;
		p = strtok(NULL,",");
	}else
		return -1;

	// UTC
	if(p){
		printf("UTC: %s\n",p);
		
		temp = atof(p);
		temp = temp/1;
		my_4g_tm->tm_sec = (int)temp%100;
		temp = temp/100;
		my_4g_tm->tm_min = (int)temp%100;
		temp = temp/100;
		my_4g_tm->tm_hour = (int)temp%100 + 8;
	
		p = strtok(NULL,",");
	}else
		return -1;

	// latitude   
	if(p){
		printf("latitude: %s\n",p);
		temp = atof(p);
		my_4g_gga->latitude = temp/100;

		index = strlen(p);
		my_4g_gga->ns_indicator=p[index-1];
		p = strtok(NULL,",");
	}else
		return -1;

	// longitude     
	if(p){
		printf("longitude: %s\n",p);
		temp = atof(p);
		my_4g_gga->longitude= temp/100;

		index = strlen(p);
		my_4g_gga->ew_indicator=p[index-1];
		p = strtok(NULL,",");
	}else
		return -1;

	// 水平精度因子
	if(p){
		printf("水平精度因子: %s\n",p);
		p = strtok(NULL,",");
	}else
		return -1;

	// 高度 
	if(p){
		printf("高度: %s\n",p);
		p = strtok(NULL,",");
	}else
		return -1;

	// 2D FIX
	if(p){
			printf("2D FIX: %s\n",p);
			p = strtok(NULL,",");
		}else
			return -1;

	// 对地导向
	if(p){
			printf("对地导向: %s\n",p);
			p = strtok(NULL,",");
		}else
			return -1;

	// 移动参数为0
	if(p){
			printf("移动参数为0: %s\n",p);
			p = strtok(NULL,",");
		}else
			return -1;

	// 移动参数为0
	if(p){
			printf("移动参数为0: %s\n",p);
			p = strtok(NULL,",");
		}else
			return -1;

	// DATE            
	if(p){
			printf("DATE: %s\n",p);
			
			temp = atof(p);
			my_4g_tm->tm_year = (int)temp%100 + 2000 - 1900;
			temp = temp/100;
			my_4g_tm->tm_mon = (int)temp%100;
			temp = temp/100;
			my_4g_tm->tm_mday = (int)temp%100;

			
			my_4g_tm->tm_yday = get_yday(my_4g_tm->tm_year,my_4g_tm->tm_mon,my_4g_tm->tm_mday);
			my_4g_tm->tm_wday = (my_4g_tm->tm_mday+2*my_4g_tm->tm_mon+3*(my_4g_tm->tm_mon+1)/5+my_4g_tm->tm_year+my_4g_tm->tm_year/4-my_4g_tm->tm_year/100+my_4g_tm->tm_year/400)%7+1;
			
			p = strtok(NULL,",");
		}else
			return -1;

	// 几颗卫星
	if(p){
			printf("几颗卫星: %s\n",p);			
			my_4g_gga->satellite = atoi(p);
			
			p = strtok(NULL,",");
		}else
			return -1;

	attr->gga = 1;
	attr->tm = 1;
			
	return 0;
}

int get_yday(int year,int mon,int day)
{
	int d,m;
	d=0;
	int team;
	team = leap(year);
	for(m=1;m<mon;m++)
		d+=monthday(m,team);
	
	day+=d;

	return day;
}

int leap(int y)
{
 int leapyear;
 if (y%4!=0)
     leapyear=0;
 else if (y%100!=0)
     leapyear=1;
 else if (y%400!=0)
     leapyear=0;
 else
     leapyear=1;
 return(leapyear);
}

int monthday(int m, int team)
{
   switch(m)
  {case 1:return(31);
   case 3:return(31);
   case 5:return(31);
   case 7:return(31);
   case 8:return(31);
   case 10:return(31);
   case 12:return(31);
   case 4:return(30);
   case 6:return(30);
   case 9:return(30);
   case 11:return(30);
   case 2:{if (team==1) {return(29);} else if (team==0) {return(28);}}}  
} 

void gps_show_tm(struct tm *tm)
{
	printf("tm->sec = %d\n", tm->tm_sec);
	printf("tm->min = %d\n", tm->tm_min);
	printf("tm->hour = %d\n", tm->tm_hour);
	printf("tm->mday = %d\n", tm->tm_mday);
	printf("tm->yday = %d\n", tm->tm_yday);
	printf("tm->wday = %d\n", tm->tm_wday);
	printf("tm->mon = %d\n", tm->tm_mon);
	printf("tm->year = %d\n", tm->tm_year);	
}


void gps_show_gga(struct lib_4g_gga *gga)
{
	printf("gga->latitude = %.7f\n", gga->latitude);
	printf("gga->ns_indicator = %c\n", gga->ns_indicator);
	printf("gga->longitude = %.7f\n", gga->longitude);
	printf("gga->ew_indicator = %c\n", gga->ew_indicator);
	printf("gga->gps_status = %d\n", gga->status);
	printf("gga->positioning_satellite = %d\n", gga->satellite);
}

void printf_warning(char *buf)
{
	printf("\n\n||||||||||-----------%s----------||||||||||\n",buf);
}
