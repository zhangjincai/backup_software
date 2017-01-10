#include <stdio.h>
#include <string.h>

int main(void)
{
	char src[13] = {"hello&world&"};
	char *str = NULL;

	str = strchr(src, '&');
	printf("str:%s\n", str);

	*str = '\0';
	printf("strlen(src):%d\n", strlen(src));

	
	return 0;
}
