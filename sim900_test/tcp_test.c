#include <stdio.h>
#include <string.h>
#include "lib_general.h"

#if 1 //阿里云
#define SERVER_IP		"112.74.97.250"
#define SERVER_PORT		60000
#else //内部服务器
#define SERVER_IP 		"113.65.161.178"
#define SERVER_PORT 	50000
#endif


int main(void)
{
	int ret = -1, sockfd = -1;
	char *tbuf[128] = {"hello world!"}, *rbuf[128] = {0};
	

	lib_begin_runtime_printf();
	
	sockfd = lib_tcp_client_nonb_new(SERVER_IP, SERVER_PORT, 1);
	if(sockfd < 0)
	{
		printf("tcp new error! sockfd:%d\n", sockfd);
		return -1;
	}
	
	ret = lib_tcp_writen(sockfd, "hello world!", strlen("hello world!"));
	if(ret <= 0)
	{
		printf("tcp write error!\n");
		return -2;
	}

	ret = lib_tcp_read_select(sockfd, rbuf, sizeof(rbuf), 5000);
	printf("read data:%s, ret:%d\n", rbuf, ret);
	
	//lib_msleep(500);
	lib_close(sockfd);	
	//shutdown(sockfd, SHUT_WR); //关闭写端
	
	lib_end_runtime_printf();

	
	return 0;
}

