
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <sys/ioctl.h>
#include <linux/if.h>

int main(void)
{
    int i,sfd,ret;
    struct ifreq req;

    sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1)  return 1;

    strcpy(req.ifr_name,"eth1");
    ret=ioctl(sfd,SIOCGIFHWADDR,&req);
    if(ret==-1)  return 2;
   
    char *ptr=req.ifr_hwaddr.sa_data;
    for(i=0;i<6;i++)
    {
       printf("%02x:",ptr[i]);
    }
    putchar('\n');
    close(sfd);
    return 0;
}
