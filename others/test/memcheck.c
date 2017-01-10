#include <stdio.h>
#include <stdlib.h>
#include "memwatch.h"

int main(void)
{
	mtrace();
	printf("hello...........\n");
	int *ptr = malloc(10*sizeof(int));
	ptr[11] = 0;
	char *p = malloc(100*sizeof(char));
	p[1000] = 0;

	free(ptr);

	return 0;
}
