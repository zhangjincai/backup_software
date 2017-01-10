
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <signal.h>

#define    PORT       10086
#define    IP         "192.168.200.250"
#define    MAX_WAIT   10

int main(void)
{
    int sfd,newfd,ret,i;
    struct sockaddr_in  myaddr,peer; //描述某台主机的信息
    socklen_t  len=sizeof(peer);

    signal(SIGCHLD,SIG_IGN);//不产生僵死进程

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
    while(1)
    {
        newfd=accept(sfd,(struct sockaddr *)&peer,&len);
        if(newfd==-1){
           perror("accept");
           continue;
        }
        printf("from:%s port:%d\n",inet_ntoa(peer.sin_addr),ntohs(peer.sin_port));
        //产生子进程服务 
        if(fork()==0)
        {
           for(i=0;i<10;i++)
           {
              ret=send(newfd,"Hello",5,MSG_NOSIGNAL);
              if(ret==-1)
              {
                 perror("write");
                 shutdown(newfd,SHUT_RDWR);
                 exit(1);
              }
              if(i%2!=0)
              {
                 ret=send(newfd,"S",1,MSG_OOB);//发送紧急数据
                 if(ret==-1)  exit(2);
                 printf("send a oob ret:%d\n",ret);
              }
              sleep(1);
           }
           //close(newfd);
           shutdown(newfd,SHUT_RDWR);
           exit(0);
        }
        //close(newfd);//parent
    }
    return 0;
}

