
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <signal.h>

#define    DEST_PORT    20000
static int sfd;

void sigfunc(int signo)
{
    struct sockaddr_in  to;
	
    to.sin_family=AF_INET;
    to.sin_port  =htons(DEST_PORT);
    //to.sin_addr.s_addr=inet_addr("192.168.1.118");
	to.sin_addr.s_addr=inet_addr("120.24.97.95");//180.76.140.185 20001
    memset(to.sin_zero,0,8);
	
    //UDP允许发送空报文 recv接收值为0    
    sendto(sfd,NULL,0,0,(struct sockaddr *)&to,sizeof(to));
    exit(0);
}

int main(void)
{
    int ret;

    signal(SIGINT,sigfunc);

    sfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sfd==-1)
    {
       perror("socket");
       return 1;
    }
    struct sockaddr_in  to;
    to.sin_family=AF_INET;
    to.sin_port  =htons(DEST_PORT);
    to.sin_addr.s_addr=inet_addr("120.24.97.95");
    memset(to.sin_zero,0,8);

    char *msg="Hello uplooking...";
    while(1)
    {
       ret=sendto(sfd,msg,strlen(msg),0,(struct sockaddr *)&to,sizeof(to));
       printf("send ret:%d\n",ret);
       sleep(2);
    }
    close(sfd);
    return 0;
}
