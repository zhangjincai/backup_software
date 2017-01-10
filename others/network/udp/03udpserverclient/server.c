
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define     SER_PORT     10086
#define     SIZE         1024

int main(void)
{
    int sfd,ret;
    struct sockaddr_in  addr,peer;
    socklen_t len=sizeof(peer);
    char buf[SIZE];
    
    signal(SIGCHLD,SIG_IGN);

    sfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sfd==-1)   return 1;
    
    addr.sin_family=AF_INET;
    addr.sin_port  =htons(SER_PORT);
    addr.sin_addr.s_addr=INADDR_ANY;
    memset(addr.sin_zero,0,8);
    ret=bind(sfd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret==-1)
    {
       perror("bind");
       return 2;
    }

    //等待连接
    while(1)
    {
       ret=recvfrom(sfd,buf,SIZE,0,(struct sockaddr *)&peer,&len);
       if(ret==-1)  continue;
       buf[ret]='\0';
       if(strcmp(buf,"PLAY")!=0)
       {
          sendto(sfd,"UNKNOW",6,0,(void *)&peer,sizeof(peer));
          continue;
       }
       printf("from:%s\n",inet_ntoa(peer.sin_addr));
       sendto(sfd,"OK",2,0,(void *)&peer,sizeof(peer));
       //请求命令正确 产生子进程 或 线程服务
       if(fork()==0)
       {
          int i;
          for(i=0;i<10;i++)
          {
             sendto(sfd,"Hello",5,0,(void *)&peer,sizeof(peer));
             sleep(2);
          }
          sendto(sfd,NULL,0,0,(void *)&peer,sizeof(peer));
          close(sfd);
          exit(0);
       }
    }
    return 0;
}
