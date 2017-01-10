
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define      SER_PORT     10086
#define      MY_IP        "192.168.1.118"

int main(void)
{
    int sockfd,ret;

    signal(SIGCHLD,SIG_IGN);
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
      perror("socket");
      return 1;
    }
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port  =htons(SER_PORT);
    addr.sin_addr.s_addr=inet_addr(MY_IP);
    ret=bind(sockfd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret==-1)
    {
      perror("bind");
      return 2;
    }
 
    if(listen(sockfd,10)==-1)
      return 3;
    
    struct sockaddr_in peer;
    int  i,newskfd;
    socklen_t  len=sizeof(peer);
    while(1)
    {
        newskfd=accept(sockfd,(struct sockaddr *)&peer,&len); 
        if(newskfd==-1)   break;
        printf("from:%s\n",inet_ntoa(peer.sin_addr));
        //产生子进程进行服务
        if(fork()==0)//child
        {
           for(i=0;i<10;i++)
           {
              write(newskfd,"hello",5);
              sleep(2);
           }
           shutdown(newskfd,SHUT_RDWR);
           exit(0);
        }
    }
    close(sockfd);
    return 0;
}  
