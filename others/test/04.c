#include <stdio.h>

int myprintf(void);

int i = 0;
int main(void)
{
	int ret = -1;
	
	while(1)
	{
		ret = myprintf();
		if(ret == 0)
			continue;
	}	
	return 0;
}

int myprintf(void)
{	
	printf("i = %d\n",i++);

	return 0;
}
