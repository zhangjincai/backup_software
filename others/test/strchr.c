#include <stdio.h>
#include <string.h>

int main(void)
{
	char msg[] = "&00000000c8", *ptr = NULL;
	int ch = '&';

	ptr = strchr(msg, ch);
	
	if(!ptr)
		printf("NULL\n");

	return 0;
}
