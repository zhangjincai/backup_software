#include "ping.h"
#include <stdio.h>

int main(void)
{
	int ret = -1, i;
	const char *ip_addr0 = "www.baidu.com", *ip_addr1 = "192.168.1.251";
	const char *ip_addrs[2] = {ip_addr0, ip_addr1};

	for(i = 0; i < 1; i++)
	{
		ret = lib_get_network_status(ip_addrs[i]);
		printf("ping ret:%d\n", ret);
	}
	
	return 0;
}

