#include <stdio.h>

int main(void)
{
		struct rent_flag{
		unsigned char b0:1;
		unsigned char b1:1;
		unsigned char b2:1;
		unsigned char b3:1;
		};

	typedef struct rent_info{
		struct rent_flag rent_flag;
	}rent_info_t;

	rent_info_t rent_info;
	
	rent_info.rent_flag.b0 = 1;
	rent_info.rent_flag.b1 = 1;
	rent_info.rent_flag.b2 = 1;
	rent_info.rent_flag.b3 = 1;
	
	printf("rent_flag:%d\n",rent_info.rent_flag);

	rent_info.rent_flag.b0 = rent_info.rent_flag & 1;
	rent_info.rent_flag.b1 = rent_info.rent_flag & 2;
	rent_info.rent_flag.b2 = rent_info.rent_flag & 4;
	rent_info.rent_flag.b3 = rent_info.rent_flag & 8;
	
	printf("rent_flag.b0:%d\n",rent_info.rent_flag.b0);
	printf("rent_flag.b1:%d\n",rent_info.rent_flag.b1);
	printf("rent_flag.b2:%d\n",rent_info.rent_flag.b2);
	printf("rent_flag.b3:%d\n",rent_info.rent_flag.b3);

	printf("rent_flag:%d\n",rent_info.rent_flag);

	return 0;
}
