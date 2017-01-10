#include <stdio.h>
#include <string.h>

int main(void)
{
	char dest[32] = {0}, *src = "hello world!";

	strncpy(dest, src, 5);
	src += 5;

	printf("src:%s\n",src);
	printf("dest:%s\n",dest);

	return 0;
}
