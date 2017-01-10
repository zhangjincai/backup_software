#include "LIB_SHA204.h"
#include <stdio.h>



int main(void)
{
	int ret = -1;
	char *version;
	unsigned char device_info[32] = {0};

	version = LIB_EV_Version();
	printf("Software Version: %s\n",version);

	ret = read_device_info(&device_info);
	lib_printf_v2("device info:",&device_info,ret,16);

	lib_sleep(2);
	ret = LIB_Encry_Verify();
	if(ret == LIB_EV_OK){
		printf("MAC Success!\n");
	}else{
		if(ret == LIB_WAKE_UP_SUCCESS)
			printf("\nwake up success!\n");
		
		printf("MAC Failed!\n");
		return -1;
	}

	
	

	return 0;
}
