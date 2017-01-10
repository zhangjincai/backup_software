
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define     MY_PORT     4444
#define     MY_BROADIP "192.168.200.255"
#define     MSG_SIZE    1024

int main(void)
{
    int sfd,ret;
    struct sockaddr_in addr,peer;
    socklen_t   len=sizeof(struct sockaddr_in);
    char buf[MSG_SIZE];

    sfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sfd==-1)  return 1;

    addr.sin_family=AF_INET;
    addr.sin_port  =htons(MY_PORT);
    addr.sin_addr.s_addr=inet_addr(MY_BROADIP);
    //addr.sin_addr.s_addr=INADDR_ANY;
    memset(addr.sin_zero,0,8);
    ret=bind(sfd,(void *)&addr,sizeof(addr));
    if(ret==-1)
    {
       perror("bind");
       return 2;
    }
    //recv
    int fd;
    fd=open("/tmp/tempfile",O_WRONLY|O_TRUNC|O_CREAT,0644);
    if(fd==-1)   return 3;

    if(fork()==0)//child
    {
       struct stat s;
       while(1)
       {
          lstat("/tmp/tempfile",&s);
          if(s.st_size>=512000)
             break;
          sleep(1);
       }
       execl("./dspplay","dspplay","/tmp/tempfile",NULL);
       perror("execl");
       exit(1);
    }
    //parent
    while(1)
    {
       ret=recvfrom(sfd,buf,MSG_SIZE,0,NULL,NULL);
       if(ret==0)   break;
       write(fd,buf,ret);
    }
    wait(NULL);
    close(sfd);
    return 0;
}
