#include <stdio.h>

#define _ROOT_PATH		"/"
#define _PATH_RESOLV	 _ROOT_PATH "etc/ppp/resolv.conf"

int main(void)
{
	char *src = "005000";

	int fee = atoi(src);
	int fen = fee % 100;
	int yuan = fee / 100;

	//sprintf(dest, "%d.%02d", yuan, fen);

	printf("fee:%d.%02d\n", yuan, fen);
	printf("%s\n",_PATH_RESOLV);	
	return 0;
}
