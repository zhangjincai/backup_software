
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>  //for ETH_P_ALL

#define    SIZE     4096

int main(void)
{
    int i,sfd,ret;
    unsigned char buf[SIZE];
    //底层套接字(原始套接字)
    sfd=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(sfd==-1)
    {
       perror("socket");
       return 1;
    }
    
    while(1)
    {
       ret=read(sfd,buf,SIZE);
       if(ret==0||ret==-1)   break;
       printf("Dmac>>");  //解析以态网头部信息
       for(i=0;i<6;i++)
       {
          printf("%02x:",buf[i]);
       }
       printf("  Smac>>");
       for(i=0;i<6;i++)
       {
          printf("%02x:",buf[i+6]);
       }
       printf(" Type:%06p\n",ntohs(*(unsigned short *)(buf+12)));
       usleep(500000);
    }
    return 0;
}
