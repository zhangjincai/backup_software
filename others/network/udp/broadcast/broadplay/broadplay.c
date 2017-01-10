
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define     BROAD_PORT       4444
#define     BROAD_IP         "192.168.200.255"
#define     SIZE             1024

void *init_file(int *size);

int main(void)
{
    int sfd,ret;
    struct sockaddr_in  dest;
    sfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sfd==-1)   return 1;

    dest.sin_family=AF_INET;
    dest.sin_port  =htons(BROAD_PORT);
    dest.sin_addr.s_addr=inet_addr(BROAD_IP);
    memset(dest.sin_zero,0,8);
    //---------开启发送广播包的支持----------------
    int flag=1;//1表示支持 0表示不支持
    ret=setsockopt(sfd,SOL_SOCKET,SO_BROADCAST,&flag,sizeof(int));
    if(ret==-1)
    {
       perror("setsockopt");
       return 2;
    }
    //---------------------------------------------
    void *addr;
    int length,temp,filesize;
    addr=init_file(&filesize);
    if(addr==MAP_FAILED || addr==NULL)
    {
       perror("mmap");
       return 5;
    }
    //---------------------------------------
    while(1)
    {
       length=0,temp=filesize;
       while(length<filesize)
       {
          if(temp>=SIZE){
            length+=sendto(sfd,addr+length,SIZE,0,(struct sockaddr *)&dest,sizeof(dest));
          }else{
            length+=sendto(sfd,addr+length,temp,0,(struct sockaddr *)&dest,sizeof(dest));
          }
          temp=filesize-length;//剩下的长度
          usleep(100);
       }
       sleep(3);
       sendto(sfd,NULL,0,0,(struct sockaddr *)&dest,sizeof(dest));
    }
    close(sfd);
    return 0;
}
void *init_file(int *size)
{
    int fd;
    fd=open("/var/ftp/pub/8.wav",O_RDONLY);
    if(fd==-1)
    {
       perror("open");
       return NULL;
    }
    struct stat s;
    if(fstat(fd,&s)==-1)
    {
       return NULL;
    }
    *size=s.st_size;
    return mmap(NULL,s.st_size,PROT_READ,MAP_PRIVATE,fd,0);
}
