
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <poll.h>

#define     SIZE      1024

int main(int argc,char *argv[])
{
    int sfd,i,ret;
    struct sockaddr_in  peeraddr;

    if(argc!=3)
    {
       fprintf(stderr,"use:[%s][PeerIp][Port]\n",argv[0]);
       return 1;
    }

    sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1)    return 2;

    peeraddr.sin_family=AF_INET;
    peeraddr.sin_port  =htons(atoi(argv[2]));
    peeraddr.sin_addr.s_addr=inet_addr(argv[1]);
    memset(peeraddr.sin_zero,0,8);
    for(i=0;i<4;i++)
    {
       ret=connect(sfd,(void *)&peeraddr,sizeof(struct sockaddr_in));
       if(ret==0)   break;
       sleep(i);
    }
    if(ret==-1)
    {
       perror("connect");
       close(sfd);
       return 3;
    }
    printf("Connect Success....\n");
    //////////////////////////////////////////////
    char buf[SIZE],msg[SIZE];
    struct pollfd  rfds[2];
    rfds[0].fd    =STDIN_FILENO;
    rfds[0].events=POLLIN;
    rfds[1].fd    =sfd;
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
          write(sfd,buf,strlen(buf));
       }
       ////////////////////////////////////
       if(rfds[1].revents & POLLIN)
       {
          ret=read(sfd,msg,SIZE);
          if(ret==0)   break;
          msg[ret]='\0';
          printf("%s",msg);
          fflush(stdout);
       }
    }
    close(sfd);
    return 0;
}

