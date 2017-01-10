
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <linux/if.h>

int main(void)
{
    int sfd,ret;
    struct ifreq req;

    sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1)  return 1;

    strcpy(req.ifr_name,"eth1");
    ret=ioctl(sfd,SIOCGIFADDR,&req);
    if(ret==-1)  return 2;
   
    struct sockaddr_in *p;
    p=(struct sockaddr_in *)(&req.ifr_addr);
    printf("ip:%s\n",inet_ntoa(p->sin_addr));
    
    getchar();
    printf("=======set eth1 ip=======\n");
    p->sin_addr.s_addr=inet_addr("10.0.0.11");
    ret=ioctl(sfd,SIOCSIFADDR,&req);
    if(ret==-1)  return 3;

    getchar();
    printf("======get MTU=======\n");
    ret=ioctl(sfd,SIOCGIFMTU,&req);
    if(ret==-1)   return 4;
    printf("MTU:%d\n",req.ifr_mtu);

    close(sfd);
    return 0;
}
