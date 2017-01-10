#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

//./app dev_name 0/1 reg_addr reg_val
int main(int argc,char *argv[])
{
	int fd;
	unsigned char cmd,reg_addr;

	if (argc < 4)
	{
		printf("Usage:%s dev_file cmd reg_addr [reg_val]\n",argv[0]);
		return 1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		perror("open dev");
		return 2;
	}
	//////////////////////////
	cmd = atoi(argv[2]);//0:read 1:write
	reg_addr = atoi(argv[3]);

	printf("cmd:%d reg_addr:%x %x\n",cmd,reg_addr);
	close(fd);
	return 0;
}

