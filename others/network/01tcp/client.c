
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define      SER_PORT      10086
#define      SER_IP        "192.168.1.118"
#define      BUF_SIZE      1024

int main(void)
{
    int sockfd,ret;
    char buf[BUF_SIZE];

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
      perror("socket");
      return 1;
    }
    struct sockaddr_in seraddr;
    seraddr.sin_family=AF_INET;
    seraddr.sin_port  =htons(SER_PORT);
    seraddr.sin_addr.s_addr=inet_addr(SER_IP);

    ret=connect(sockfd,(struct sockaddr *)&seraddr,sizeof(seraddr));
    if(ret==-1)
    {
       perror("connect");
       return 2;
    }
    printf("Connect Server Success\n");
    
    while(1)
    {
       ret=read(sockfd,buf,BUF_SIZE);
       if(ret==0)    break;//distconnect
       buf[ret]='\0';
       printf("===>%s\n",buf);
    } 
    close(sockfd);
    return 0;
}  
