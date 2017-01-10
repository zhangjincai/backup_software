
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>  //for ETH_P_ALL

#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <linux/if.h>
#include <linux/if_packet.h>

#include "mypacket.h"

#define    SIZE     4096

int main(void)
{
    int i,sfd,ret;
    unsigned char buf[SIZE]={0};
    //底层套接字(原始套接字)
    sfd=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(sfd==-1)
    {
       perror("socket");
       return 1;
    }
    ///////////////////////////////////////////////
    struct ifreq  req;
    strcpy(req.ifr_name,"eth0");//指定接口名称
    ret=ioctl(sfd,SIOCGIFINDEX,&req);//get index
    if(ret==-1)   return 2;
    struct sockaddr_ll addr;
    addr.sll_family  =AF_PACKET;
    addr.sll_protocol=htons(ETH_P_ALL);
    addr.sll_ifindex =req.ifr_ifindex;
    ret=bind(sfd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret==-1)
    {
       perror("bind");
       return 3;
    }
    //////////  make packets   ///////////////////
    ETH_T eth;
    ARP_T arp;
    struct who one={
      .Dmac={0xff,0xff,0xff,0xff,0xff,0xff},
      .Smac={0x00,0x0C,0x29,0x0E,0x14,0x0E},
    };
    one.Dip=inet_addr("192.168.2.111");
    one.Sip=inet_addr("192.168.200.250");    

    make_eth(&eth,0x0806,one);
    make_arp(&arp,1,one);
    memcpy(buf,&eth,sizeof(ETH_T));
    memcpy(buf+sizeof(ETH_T),&arp,sizeof(ARP_T));
    strcpy(buf+sizeof(ETH_T)+sizeof(ARP_T),"Hello Arp");
    //-------------------------------------------
    while(1)
    {
      ret=sendto(sfd,buf,100,0,(struct sockaddr *)&addr,sizeof(addr));
      if(ret==-1)
      {
         perror("sendto");
         break;
      }
      printf("send to ret:%d\n",ret);
      sleep(1);
    }
    close(sfd);
    return 0;
}
