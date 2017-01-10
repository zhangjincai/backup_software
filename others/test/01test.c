#include <stdio.h>

int main(void)
{

	float a = 77.8897;
	int b = 65536;
	
	int c = 0x22;
	int d = 01;
	unsigned short num = 0x9000;

	if(num != 0x9000)
		printf("not same\n");
	else printf("is same\n");
	
	printf("a = %f,	%e, %g\n",a,a,a);
	printf("c = %#x, d = %#o",c, d);	
	return 0;
}
