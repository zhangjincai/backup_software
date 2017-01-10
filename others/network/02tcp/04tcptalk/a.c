
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <poll.h>

#define     SIZE      1024

int init_socket(char *ip,short port);

int main(int argc,char *argv[])
{
    int sfd,newfd,ret;
    struct sockaddr_in  peer;
    socklen_t   len=sizeof(struct sockaddr_in);

    if(argc!=3)
    {
       fprintf(stderr,"use:[%s][MyIp][Port]\n",argv[0]);
       return 1;
    }
    sfd=init_socket(argv[1],atoi(argv[2]));
    if(sfd<0)
    {
       fprintf(stderr,"init socket failed\n");
       return 2;
    }
    ///////////////////////////////////////////
    while(1)
    {
       newfd=accept(sfd,(struct sockaddr *)&peer,&len);
       if(newfd>0)  break;
    }
    close(sfd);//只与一个人相联

    char buf[SIZE],msg[SIZE];
    struct pollfd  rfds[2];
    rfds[0].fd    =STDIN_FILENO;
    rfds[0].events=POLLIN;
    rfds[1].fd    =newfd;
    rfds[1].events=POLLIN;
    while(1)
    {
       poll(rfds,2,-1);
       ///////////////////////////////////
       if(rfds[0].revents & POLLIN)
       {
          fgets(buf,SIZE,stdin);
          if(!strncmp(buf,"exit",4))
          {
              break;
          }
          write(newfd,buf,strlen(buf));
       }
       ////////////////////////////////////
       if(rfds[1].revents & POLLIN)
       {
          ret=read(newfd,msg,SIZE);
          if(ret==0)   break;
          msg[ret]='\0';
          printf("\033[31m%s\033[0m\n  %s",inet_ntoa(peer.sin_addr),msg);
       }
    }
    close(newfd);
    return 0;
}
int init_socket(char *ip,short port)
{
    int temp,ret;
    struct sockaddr_in   addr;
    temp=socket(AF_INET,SOCK_STREAM,0);
    if(temp==-1)    return -1;

    addr.sin_family=AF_INET;
    addr.sin_port  =htons(port);
    addr.sin_addr.s_addr=inet_addr(ip);
    memset(addr.sin_zero,0,8);
    ret=bind(temp,(struct sockaddr *)&addr,sizeof(addr));
    if(ret==-1)
    {
      perror("bind");
      close(temp);
      return -2;
    }
    if(listen(temp,10)==-1)
    {
       perror("listen");
       close(temp);
       return -3;
    }
    return temp;
}

