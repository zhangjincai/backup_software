
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(void)
{
    struct sockaddr_in   ip;

    ip.sin_addr.s_addr=inet_addr("192.168.2.30");


    printf("ip:%s\n",inet_ntoa(ip.sin_addr));
    return 0;
}
