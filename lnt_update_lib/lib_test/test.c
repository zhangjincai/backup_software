#include <stdio.h>
#include "lib_reader_update.h"
#include <pthread.h>


//要升级的读卡器固件文件
#if 1
#define	FIRMWARE "/mnt/mount/lnt_update_lib/firmware/LNT_ZM_V1.1_20160219.lfd"
#else 
#define	FIRMWARE "/mnt/mount/lnt_update_lib/firmware/LNT_ZM_V1.1_20151127.lfd"
#endif


void *run(void *arg)
{	
	int ret = -1;	
	int fd = *(int *)arg;

	//printf("haha--------------\n");
	while(1)
	{	
		#if 0
		ret = lib_get_update_status();
		printf("lib_get_update_status, ret:%d\n", ret);
		if(ret == READER_UPDATE_ING)
		{
			printf("reader updating......\n");
			return (void *)(-1);
		}
		lib_sleep(1);
		#endif
		
		ret = lnt_reader_update(fd, FIRMWARE);	
		if(ret >= 1)
			break;
	}

	pthread_exit((void *)0);
	return (void *)ret;
}

int main(int argc,char *argv[])
{	
	int ret = -1, fd = -1;	
	char *version;	
	time_t begin_times, end_times;
	unsigned int consume_time = 0;//升级耗时

	time(&begin_times);

	version = lib_update_version();	
	fprintf(stderr,"\nsoftware version:%s\n",version);		

#if 1
	fd = lnt_init();//岭南通模块初始化(上电 串口初始化 获取软件版本)
	if(fd < 0){
		fprintf(stderr,"lnt_init failed!\n");
		return fd;
	}
#endif
#if 1
	pthread_t tid;
	ret = pthread_create(&tid, NULL, run, &fd);
	printf("pthread_create ret:%d\n",ret);
	lib_sleep(40);
#else
	ret = lnt_reader_update(fd, FIRMWARE);		
#if 1
	if(ret == LIB_UPDATE_OK){		
		fprintf(stderr,"\t恭喜您,固件升级成功啦!\n");	
	}else{		
		fprintf(stderr,"\t抱歉,固件升级失败啦!\n");	
	}	
#endif
#endif
	time(&end_times);
	consume_time = end_times - begin_times;
	printf("Update takes %d seconds.\n",consume_time);//升级耗时

#if 0
	ret = lib_get_update_status();
	printf("----------ret:%d\n", ret);
#endif

	return ret;
}


