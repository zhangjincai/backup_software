
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define     SER_PORT     10086
#define     SER_IP       "192.168.200.250"
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
    addr.sin_port  =htons(SER_PORT);
    addr.sin_addr.s_addr=inet_addr(SER_IP);
    memset(addr.sin_zero,0,8);

    //请求联接
    while(1)
    {
       sendto(sfd,"PLAY",4,0,(void *)&addr,sizeof(addr));
       ret=recvfrom(sfd,buf,MSG_SIZE,0,NULL,NULL);
       if(ret==-1 || ret==0)  return 3;//error!!!
       buf[ret]='\0';
       if(strcmp(buf,"OK")==0)  break;
    }
    printf("Connect Success...\n");
    //recv
    while(1)
    {
       ret=recvfrom(sfd,buf,MSG_SIZE,0,NULL,NULL);
       if(ret==0 || ret==-1)   break;
       buf[ret]='\0';
       printf(">>%s\n",buf);
    }
    close(sfd);
    return 0;
}
