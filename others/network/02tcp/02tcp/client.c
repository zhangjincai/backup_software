
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define    SIZE          1024

#define    SER_PORT      10086
#define    SER_IP        "192.168.200.250"

int main(void)
{
    int i,sfd,ret;
    struct sockaddr_in  seraddr;

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
    char msg[SIZE];
    while(1)
    {
       ret=read(sfd,msg,SIZE);
       if(ret==0)   break;
       msg[ret]='\0';
       printf("server say>%s\n",msg);
    }
    close(sfd);
    return 0;
}
