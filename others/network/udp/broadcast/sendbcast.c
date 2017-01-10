
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define     BROAD_PORT       4444
#define     BROAD_IP         "192.168.200.255"

int main(void)
{
    int sfd,ret;
    struct sockaddr_in  dest;
    sfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sfd==-1)   return 1;

    dest.sin_family=AF_INET;
    dest.sin_port  =htons(BROAD_PORT);
    dest.sin_addr.s_addr=inet_addr(BROAD_IP);
    memset(dest.sin_zero,0,8);
    //---------开启发送广播包的支持----------------
    int flag=1;//1表示支持 0表示不支持
    ret=setsockopt(sfd,SOL_SOCKET,SO_BROADCAST,&flag,sizeof(int));
    if(ret==-1)
    {
       perror("setsockopt");
       return 2;
    }
    //---------------------------------------------

    char *msg="hello this is broad cast data";
    while(1)
    {
       ret=sendto(sfd,msg,strlen(msg),0,(struct sockaddr *)&dest,sizeof(dest));
//       printf("sendto ret:%d\n",ret);
       if(ret==-1)
       {
          perror("send broad cast");
          break;
       }
       sleep(2);
    }
    close(sfd);
    return 0;
}
