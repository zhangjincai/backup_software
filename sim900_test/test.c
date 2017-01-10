#include <stdio.h>
#include "lib_general.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <fcntl.h>


#define SIM900_UART		"/dev/ttyUSB3"
#define READ_SLEEP		100


int main(void)
{
		//printf("sim900 test ...\n");
	
#if 1 
		int ret = -1;
		char tbuf[128] = {0},rbuf[128] = {0};
		lib_serial_t sim900;
		
		memset(&sim900, 0, sizeof(lib_serial_t));
		//串口初始化
		strcpy(sim900.pathname,SIM900_UART);
		sim900.flags = O_RDWR;
		sim900.speed = 9600;
		sim900.databits = 8;
		sim900.stopbits = 1;

		ret = lib_serial_init(&sim900);
		if(ret == -1)
		{
			perror("serial init");
		}
		printf("uart init ret:%d, sfd:%d\n", ret, sim900.sfd);
#endif
		#if 0 //回显
		ret = lib_serial_send(sim900.sfd, "ATE1\r", strlen("ATE1\r"));

		ret = lib_serial_readn_select(sim900.sfd, &rbuf, sizeof(rbuf), 200);

		printf("read data:%s\n", rbuf);
		#endif

		//lib_begin_runtime_printf();
	
		ret = lib_tcp_connect_setup(sim900.sfd, "112.74.97.250", "60000");

		//lib_end_runtime_printf();

		//lib_sleep(2);
	#if 0 //关闭TCP连接
		ret = lib_serial_send(sim900.sfd, "AT+CIPCLOSE=1\r", strlen("AT+CIPCLOSE=1\r"));

		ret = lib_serial_readn_select(sim900.sfd, &rbuf, sizeof(rbuf), 200);
		printf("read data:%s\n", rbuf);

		ret = lib_serial_send(sim900.sfd, "AT+CIPSHUT\r", strlen("AT+CIPSHUT\r"));

		ret = lib_serial_readn_select(sim900.sfd, &rbuf, sizeof(rbuf), 200);
		printf("read data:%s\n", rbuf);
		
	#endif

		
		
		//shutdown(sim900.sfd, SHUT_WR); //关闭写端

		lib_close(sim900.sfd);
		
		return 0;
}

#define READ_DATA	1
int lib_tcp_connect_setup(int sfd, const char *ip, const char *port)
{
	int ret = -1, i = 0;
	unsigned char cmd[128] = {0}, data[128] = {0};

	for(i = 0; i < 2; i++)
	{
		if(i == 0)
		{
		#if 1 //初始化
			//设置移动台台类别为 B
			strcpy(cmd, "AT+CGCLASS=\"B\"\r"); 
			ret = lib_serial_send(sfd, cmd, strlen(cmd));
			#if READ_DATA
			ret = lib_serial_readn_select(sfd, &data, sizeof(data), READ_SLEEP);
			printf("read data:%s\n", data);
			#endif
			memset(cmd, 0, sizeof(cmd));
			memset(data, 0, sizeof(data));

			//设置 PDP 上下文标标志为 1，采用IP协议，接入点为"CMNET"
			strcpy(cmd, "AT+CGDCONT=1,\"IP\",\"CMNET\"\r");
			ret = lib_serial_send(sfd, cmd, strlen(cmd));
			#if READ_DATA
			ret = lib_serial_readn_select(sfd, &data, sizeof(data), READ_SLEEP);
			printf("read data:%s\n", data);
			#endif
			memset(cmd, 0, sizeof(cmd));
			memset(data, 0, sizeof(data));

			//设置附着 GPRS 业务
			strcpy(cmd, "AT+CGATT=1\r");
			ret = lib_serial_send(sfd, cmd, strlen(cmd));
			#if READ_DATA
			ret = lib_serial_readn_select(sfd, &data, sizeof(data), READ_SLEEP);
			printf("read data:%s\n", data);
			#endif
			memset(cmd, 0, sizeof(cmd));
			memset(data, 0, sizeof(data));

			//设置为 GPRS 连接，接入点为”CMNET”
			strcpy(cmd, "AT+CIPCSGP=1,\"CMNET\"\r");
			ret = lib_serial_send(sfd, cmd, strlen(cmd));
			#if READ_DATA
			ret = lib_serial_readn_select(sfd, &data, sizeof(data), READ_SLEEP);
			printf("read data:%s\n", data);
			#endif
			memset(cmd, 0, sizeof(cmd));
			memset(data, 0, sizeof(data));

			//设置 TCP 连接本地端口号
			strcpy(cmd, "AT+CLPORT=\"TCP\",\"2000\"\r");
			ret = lib_serial_send(sfd, cmd, strlen(cmd));
			#if READ_DATA
			ret = lib_serial_readn_select(sfd, &data, sizeof(data), READ_SLEEP + 300);
			printf("read data:%s\n", data);
			#endif
			memset(cmd, 0, sizeof(cmd));
			memset(data, 0, sizeof(data));
		#endif
		}

		lib_begin_runtime_printf();
		//设置TCP服务器IP和端口	
		strcpy(cmd, "AT+CIPSTART=\"TCP\",");
		strcat(cmd, "\"");
		strcat(cmd, ip);
		strcat(cmd, "\"");
		strcat(cmd, ",");
		strcat(cmd, "\"");
		strcat(cmd, port);
		strcat(cmd, "\"\r");
		ret = lib_serial_send(sfd, cmd, strlen(cmd));
		#if READ_DATA
		ret = lib_serial_readn_select(sfd, &data, sizeof(data), READ_SLEEP + 3000);
		printf("read data:%s\n", data);
		#endif
		memset(cmd, 0, sizeof(cmd));
		memset(data, 0, sizeof(data));
		
		//进入数据发送模式
		strcpy(cmd, "AT+CIPSEND\r");
		ret = lib_serial_send(sfd, cmd, strlen(cmd));
		#if READ_DATA
		ret = lib_serial_readn_select(sfd, &data, sizeof(data), READ_SLEEP + 100);
		printf("read data:%s\n", data);
		#endif
		memset(cmd, 0, sizeof(cmd));
		memset(data, 0, sizeof(data));

		//发送数据
		strcpy(cmd, "hello world"); //send data
		ret = lib_serial_send(sfd, cmd, strlen(cmd));
		#if READ_DATA
		ret = lib_serial_readn_select(sfd, &data, sizeof(data), READ_SLEEP);
		printf("read data:%s\n", data);
		#endif
		memset(cmd, 0, sizeof(cmd));
		memset(data, 0, sizeof(data));

		//启动数据发送
		cmd[0] = 0x1A;
		ret = lib_serial_send(sfd, cmd, 1);
		#if READ_DATA
		ret = lib_serial_readn_select(sfd, &data, sizeof(data), READ_SLEEP);
		printf("read data:%s\n", data);
		#endif
		memset(cmd, 0, sizeof(cmd));
		memset(data, 0, sizeof(data));

	#if 1 
		//关闭TCP连接
		strcpy(cmd, "AT+CIPCLOSE=1\r"); //send data
		ret = lib_serial_send(sfd, cmd, strlen(cmd));
		#if READ_DATA
		ret = lib_serial_readn_select(sfd, &data, sizeof(data), READ_SLEEP + 100);
		printf("read data:%s\n", data);
		#endif
		memset(cmd, 0, sizeof(cmd));
		memset(data, 0, sizeof(data));

		#if 1
		//关闭移动场景
		strcpy(cmd, "AT+CIPSHUT\r"); //send data
		ret = lib_serial_send(sfd, cmd, strlen(cmd));
		#if READ_DATA
		ret = lib_serial_readn_select(sfd, &data, sizeof(data), READ_SLEEP + 100);
		printf("read data:%s\n", data);
		#endif
		#endif
	#endif
		lib_end_runtime_printf();
	
		lib_sleep(2);
		printf("-------------------------------------\n");
	}

		
	return 0;
}
