#include <stdio.h>
#include <string.h>


int main(void)
{
	char *str1 = "hello world", *str2 = "w", *ret;

	printf("str1:%s\n",str1);
	ret = strstr(str1,str2);
	//ret = strchr(str1,str2);
	printf("str1:%s\n",str1);
	printf("ret:%s\n",ret);

	return 0;
}
