
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

unsigned int hex2i(char *line);

//./app dev_name 0/1 reg_addr reg_val
int main(int argc , char *argv[])
{
	int fd,cmd;
	unsigned char reg_addr,reg_val,buffer[2];

	if (argc < 4)
	{
		printf("Usage:%s dev_file 0/1(read/write) reg_addr [reg_val]\n",argv[0]);
		return 1;
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		perror("open dev");
		return 1;
	}
	//////////////////////////
	cmd = atoi(argv[2]);
	if (strncmp(argv[3], "0x", 2) != 0)		
		reg_addr = atoi(argv[3]);
	else
		reg_addr = hex2i(argv[3]);		

	printf("cmd:%d reg_addr:%x\n",cmd,reg_addr);
	if(argc == 5)
	{
        	if (strncmp(argv[4], "0x", 2) != 0)
        		reg_val = atoi(argv[4]);
        	else
        		reg_val = hex2i(argv[4]);
		printf("reg_val:%x\n",reg_val);		
	}

	buffer[0] = reg_addr;
	buffer[1] = reg_val;
	if (cmd == 0)//read
	{
		//write(fd,&reg_addr,sizeof(reg_addr));
		//usleep(10000);
		read(fd,&buffer[0],sizeof(buffer[0]));
		usleep(10000);
		//read(fd,&buffer[0],sizeof(buffer[0]));
		printf("read reg_val:0x%x\n",buffer[0]);
	}
	else//write
	{
		write(fd,buffer,sizeof(buffer));
		//write(fd,&reg_addr,sizeof(reg_addr));
		usleep(10000);
		//write(fd,&reg_val,sizeof(reg_val));
	}	

	close(fd);
	return 0;
}

unsigned int hex2i(char *line)
{
	int i, ch;
	unsigned int val = 0, tmp = 0;

	// 0x352 = 16^2*3+ 16^1*5 + 16^0*2
	line += 2; // clear 0x

	while (*line)
	{
		tmp = 1;
		for (i = 0; i < strlen(line)-1; i++)
			tmp *= 16;
		
		switch (*line)
		{
			case '0'...'9':
				val += tmp*(*line - '0');
				break;
			case 'a'...'f':
				val += tmp*(*line-'a'+10);
				break;
			case 'A'...'F':
				val += tmp*(*line-'A'+10);
				break;
		}
		line++;
	}
	return val;
}
