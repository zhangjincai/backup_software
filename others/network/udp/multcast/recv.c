
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/if.h>


#define     MULT_PORT    4444
#define     MULT_IP      "224.4.4.4"
#define     MY_IP        "192.168.200.250"
#define     MSG_SIZE     1024

int main(void)
{
    int sfd,ret;
    struct sockaddr_in addr,peer;
    socklen_t   len=sizeof(struct sockaddr_in);
    char buf[MSG_SIZE];

    sfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sfd==-1)  return 1;

    addr.sin_family=AF_INET;
    addr.sin_port  =htons(MULT_PORT);
    //addr.sin_addr.s_addr=INADDR_ANY;
    addr.sin_addr.s_addr=inet_addr(MULT_IP);
    memset(addr.sin_zero,0,8);
    ret=bind(sfd,(void *)&addr,sizeof(addr));
    if(ret==-1)
    {
       perror("bind");
       return 2;
    }
    //-----------------加入至某个多播组-------------
    struct ifreq  req;
    strcpy(req.ifr_name,"eth0");//指定接口名称
    ret=ioctl(sfd,SIOCGIFINDEX,&req);
    if(ret==-1)
    {
       printf("get index by name failed\n");
       return 3;
    }
    struct ip_mreqn mreq;
    mreq.imr_multiaddr.s_addr=inet_addr(MULT_IP);
    mreq.imr_address.s_addr =inet_addr(MY_IP);
    mreq.imr_ifindex=req.ifr_ifindex;
    ret=setsockopt(sfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq));
    if(ret==-1)
    {
       perror("setsockopt");
       return 3;
    }
    ///////////////////////////////////////////////
    //recv
    while(1)
    {
       ret=recvfrom(sfd,buf,MSG_SIZE,0,(void *)&peer,&len);
       if(ret==0 || ret==-1)   break;
       buf[ret]='\0';
       printf("%s>>%s\n",inet_ntoa(peer.sin_addr),buf);
    }
    close(sfd);
    return 0;
}
