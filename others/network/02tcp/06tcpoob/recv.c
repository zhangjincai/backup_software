
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

#define    SIZE          1024

#define    SER_PORT      10086
#define    SER_IP        "192.168.200.250"

int sfd;

void recvoob(int signo)
{
    if(signo!=SIGURG)   return;
    char ret,buf[32];
    ret=recv(sfd,buf,32,MSG_OOB);
    if(ret==-1)
    {
       perror("recv oob");
       exit(5);
    }
    buf[ret]='\0';
    printf("====>oob ret:%d :%s\n",ret,buf);
}

int main(void)
{
    int i,ret;
    struct sockaddr_in  seraddr;

    signal(SIGURG,recvoob);

    sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1)
    {
      perror("socket");
      return 1;
    }
    seraddr.sin_family=AF_INET;
    seraddr.sin_port  =htons(SER_PORT);
    seraddr.sin_addr.s_addr=inet_addr(SER_IP);
    memset(seraddr.sin_zero,0,8);
    /////////////////////////////////////////////////
    //超时重连
    for(i=0;i<8;i++)
    {
       ret=connect(sfd,(struct sockaddr *)&seraddr,sizeof(seraddr));
       if(ret==0)  break; //success
       sleep(i);
    }
    if(ret==-1)
    {
       perror("connect");
       return 2;
    }
    printf("Connect Success....\n");
    ////////////////////////////////////////////////////

    //-------------开启sfd信号I/O的支持--------
    fcntl(sfd,F_SETOWN,getpid());
    char msg[SIZE];
    while(1)
    {
       ret=recv(sfd,msg,SIZE,0);//read
       if(ret==0)   break;
       msg[ret]='\0';
       printf("server say>%s\n",msg);
    }
    close(sfd);
    return 0;
}
