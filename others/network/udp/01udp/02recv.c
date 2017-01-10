
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define     MY_PORT     10086
#define     MY_IP       "192.168.1.118"
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
    addr.sin_addr.s_addr=inet_addr(MY_IP);
    //addr.sin_addr.s_addr=INADDR_ANY;
    memset(addr.sin_zero,0,8);
	
    ret=bind(sfd,(void *)&addr,sizeof(addr));
    if(ret==-1)
    {
       perror("bind");
       return 2;
    }
    //recv
    while(1)
    {
       //ret=recvfrom(sfd,buf,MSG_SIZE,0,NULL,NULL);
       ret=recvfrom(sfd,buf,MSG_SIZE,0,(void *)&peer,&len);
       if(ret==0 || ret==-1)   break;
       buf[ret]='\0';
       printf("%s>>%s\n",inet_ntoa(peer.sin_addr),buf);
    }
    close(sfd);
    return 0;
}
