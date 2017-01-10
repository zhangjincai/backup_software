#include <stdio.h>  
  
#include <sys/types.h>  
  
int main(void)  
{  
  
	int x=0x12345678; /* 305419896 */  
  
	unsigned char *xp=(char *)&x;  
  
	printf("%0x % 0x %0x %0x",xp[0],xp[1],xp[2],xp[3]);  
  
	return 0;
}  
