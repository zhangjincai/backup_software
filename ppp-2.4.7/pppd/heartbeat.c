#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib_general.h"
#include "heartbeat.h"


#define HB_APP_ERR

#ifdef HB_APP_ERR
#define HB_ERR(fmt, args...)	 do{ \
	fprintf(stderr, "ERROR: %s:%d, ", __FUNCTION__,__LINE__); \
	fprintf(stderr, fmt, ##args); \
}while(0)
#else
#define HB_ERR(fmt, args...)
#endif

#define HB_UNIX_DOMAIN			"/tmp/pppd_hb_unix.domain"
#define HB_PERM					(0777)

static int g_unix_sockfd = -1;



struct hb_unix_package
{
	unsigned char id;
	unsigned char stat;
	unsigned char result;
	unsigned char rsv;
}__attribute__((packed));



static void *hb_handle_thread(void *arg);




int heartbeat_run(void)
{
	pthread_t hd_thr;

	g_unix_sockfd = lib_unix_server_new(HB_UNIX_DOMAIN, HB_PERM, 5);
	if(g_unix_sockfd == LIB_GE_ERROR)
	{
		HB_ERR("unix server new failed!\n");
		return -1;
	}

	lib_normal_thread_create(&hd_thr, hb_handle_thread, NULL);
	
	fprintf(stderr, "heartbeat running\n");
	
	return 0;
}

static void *hb_handle_thread(void *arg)
{
	int cli_sockfd = -1;
	struct sockaddr_un cli_addr;
	struct hb_unix_package pkg;
		
	while(1)
	{
		fprintf(stderr, "hb handle thread runing\n");
		
		memset(&cli_addr, 0, sizeof(struct sockaddr_un));
		memset(&pkg, 0, sizeof(struct hb_unix_package));
		
		cli_sockfd = lib_unix_accept(g_unix_sockfd, &cli_addr);
		if(cli_sockfd > 0)
		{
			lib_readn(cli_sockfd, &pkg, sizeof(struct hb_unix_package));	
			pkg.result = 1;
			lib_writen(cli_sockfd, &pkg, sizeof(struct hb_unix_package));	

			lib_unix_close(cli_sockfd);
		}
		else
			lib_sleep(2);
	}


	return lib_thread_exit((void *)NULL);
}




