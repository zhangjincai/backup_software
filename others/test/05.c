#include <stdio.h>
#include <string.h>

int main(void)
{	
	unsigned char dest[128] = {0}, src[100] = {0},*ptr = NULL;

	ptr = memcpy(&dest,&src,128);
	if(NULL == ptr)
		printf("memcpy error!\n");
	
	return 0;
}
