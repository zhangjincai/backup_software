#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include "gpio_ctrl.h"


int main(int argc, char *argv[])
{
	int fd;

	fprintf(stderr, "GPIO CTRL APP, Software Compiled Time: %s, %s.\n", __DATE__, __TIME__);
	
	fd = open("/dev/gpio_ctrl", O_RDWR);
	if(fd < 0)
	{
		printf("open failed!\n");
		return 0;
	}

	//ioctl(fd, GPIO_CTRL_SET_BACKLIGHTEN, GPIO_CTRL_SW_ON);//Òº¾§ÆÁ
	ioctl(fd, GPIO_CTRL_SET_PWR_2232_2, GPIO_CTRL_SW_OFF);//ch3410g
	//ioctl(fd, GPIO_CTRL_SET_PWR_2232_1, GPIO_CTRL_SW_OFF);//ft2232d
	//ioctl(fd, GPIO_CTRL_SET_PWR_3G, GPIO_CTRL_SW_OFF);    //3G
	//ioctl(fd, GPIO_CTRL_SET_PWR_YCT, GPIO_CTRL_SW_OFF);  //ÁëÄÏÍ¨¶Á¿¨Æ÷ÉÏµç
	
	close(fd);

	return 0;
}




