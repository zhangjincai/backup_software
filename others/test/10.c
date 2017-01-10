#include <stdio.h>

int main(void)
{
struct ndev_status
{
	unsigned char power:1;				//电源电压状态
	unsigned char rtc:1;				//RTC状态
}__attribute__((packed));
typedef struct ndev_status ndev_status_t;

	printf("%d\n",sizeof(ndev_status_t));

	return 0;
}
