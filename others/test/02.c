#include <stdio.h>
#include <string.h>

int main(void)
{
	unsigned char str[] = "how,are,you!";
	char *buf, *delim = ", ";
	#if 0
	buf = strtok(&str,delim);
	printf("buf:%s\n",buf);

	buf = strtok(NULL,delim);
	printf("buf:%s\n",buf);
	
	printf("sizeof(void *):%d\n", sizeof(void *));
	#endif

	char *str1;
	memset(&str1, 0, sizeof(str1));
	printf("strlen(str1):%d\n", strlen(str1));

	return 0;
}
