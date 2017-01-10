
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <linux/if.h>
#include <sys/ioctl.h>

#define     MULT_IP     "224.4.4.4" //D类IP为组播地址
#define     MULT_PORT    4444
#define     MY_IP        "192.168.200.250"

int main(void)
{
    int  sfd,ret;
    struct sockaddr_in  to;
    sfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sfd==-1)    return 1;
    
    to.sin_family=AF_INET;
    to.sin_port  =htons(MULT_PORT);
    to.sin_addr.s_addr=inet_addr(MULT_IP);
    memset(to.sin_zero,0,8);
//----如果默认不支持发送多播时 指定多播报文接口-------
    struct ip_mreqn mreq;
    struct ifreq   req;
    strcpy(req.ifr_name,"eth0");
    if(ioctl(sfd,SIOCGIFINDEX,&req)==-1)
    {
       fprintf(stderr,"get index by name failed\n");
       return 3;
    }
    mreq.imr_address.s_addr=inet_addr(MY_IP);
    mreq.imr_multiaddr.s_addr=inet_addr(MULT_IP);
    mreq.imr_ifindex=req.ifr_ifindex;
    ret=setsockopt(sfd,IPPROTO_IP,IP_MULTICAST_IF,&mreq,sizeof(mreq));
    if(ret==-1)
    {
       fprintf(stderr,"指定多播报文接口失败!!!!\n");
       return 4;
    }
    //////////////////////////////////////////////////
    char *msg="test multicast data....";
    while(1)
    {
       ret=sendto(sfd,msg,strlen(msg),0,(struct sockaddr *)&to,sizeof(to));
       printf("sendto ret:%d\n",ret);
       sleep(2);
    }
    close(sfd);
    return 0;
}
