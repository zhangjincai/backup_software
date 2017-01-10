
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <signal.h>

#define    PORT       10086
#define    IP         "192.168.200.250"
#define    MAX_WAIT   10

void sigfunc(int signo)
{
    printf("a sig send...\n");
}

int main(void)
{
    int sfd,newfd,ret,i;
    struct sockaddr_in  myaddr,peer; //描述某台主机的信息
    socklen_t  len=sizeof(peer);

    signal(SIGPIPE,sigfunc);

    sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1)
    {
       perror("socket");
       return 1;
    }
    //把当前套接字绑在某个固的端口
    myaddr.sin_family=AF_INET;
    myaddr.sin_port  =htons(PORT);
    //myaddr.sin_addr.s_addr=inet_addr(IP);
    myaddr.sin_addr.s_addr=INADDR_ANY;//本机的所有IP
    memset(myaddr.sin_zero,0,8);//填补字节
    ret=bind(sfd,(struct sockaddr *)&myaddr,sizeof(myaddr));
    if(ret==-1)
    {
      perror("bind");
      return 2;
    }
    if(listen(sfd,MAX_WAIT)==-1)
    {
       perror("listen");
       return 3;
    }
    //处理连接  如果没有连接则阻塞等待
    //newfd=accept(sfd,NULL,NULL);    
    newfd=accept(sfd,(struct sockaddr *)&peer,&len);
    if(newfd==-1){
       perror("accept");
       return 4;
    }
    close(sfd);//只想跟一个人通信 不再需要监听

    printf("from:%s port:%d\n",inet_ntoa(peer.sin_addr),ntohs(peer.sin_port));
    
    for(i=0;i<10;i++)
    {
       ret=write(newfd,"Hello",5);
       if(ret==-1)
       {
          perror("write");
          return 5;
       }
       sleep(1);
    }
    close(newfd);
    return 0;
}




