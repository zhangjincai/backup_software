#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ftp.h"

char databuf[1024];

void gettime(void)
{
	time_t now=0;
	struct tm *p_localtime;
	
	time(&now);
	p_localtime = localtime(&now);
	sprintf(databuf, "%04d-%02d-%02d %02d:%02d:%02d	test.\r\n",
	p_localtime->tm_year+1900,p_localtime->tm_mon+1,p_localtime->tm_mday,
	p_localtime->tm_hour,p_localtime->tm_min,p_localtime->tm_sec);
	printf("%s", databuf);
}

int main(void)
{	
    gettime();
    
	if(SendDataToFTP("jincai", "314159", "test.txt", databuf, strlen(databuf))<0)
	{
		printf("Send file Failed!!!!!!\n");
		return -1;
	}
	printf("Send file Success!\n");
    
    return 0;
}

