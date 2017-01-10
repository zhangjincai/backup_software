//模拟简单ftp登陆过程
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define   FTP_PORT     21
#define   SIZE         1024

void recv_msg(int sockfd,char buf[]);
void send_msg(int sockfd,char buf[]);

int main(int argc,char *argv[])
{
    char *ip="127.0.0.1";
    int  sfd,ret;
    struct sockaddr_in addr;

    if(argv[1]!=NULL)  ip=argv[1];

    sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1)
    {
       perror("socket");
       return 1;
    }
    addr.sin_family=AF_INET;
    addr.sin_port  =htons(FTP_PORT);
    addr.sin_addr.s_addr=inet_addr(ip);
    memset(addr.sin_zero,0,8);
    
    ret=connect(sfd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret==-1)
    {
        perror("Connect");
        close(sfd);
        return 2;
    }
    /////////////////////////////////////////////
    char *t,buf[SIZE],temp[SIZE];
    printf("Connected to %s.\n",ip);
    recv_msg(sfd,buf);
    printf("%s",buf);

    printf("Name (%s:root):",ip);
    fgets(temp,SIZE,stdin);
    t=strchr(temp,'\n');
    if(t!=NULL) *t='\0';

    snprintf(buf,SIZE,"USER %s\n",temp);//"USER name\n"
    send_msg(sfd,buf);
    recv_msg(sfd,buf);
    printf("%s",buf);
    //////////////////////////////////////////
    printf("Passwd:");
    fgets(temp,SIZE,stdin);
    t=strchr(temp,'\n');
    if(t!=NULL) *t='\0';
    
    snprintf(buf,SIZE,"PASS %s\n",temp);
    send_msg(sfd,buf);
    recv_msg(sfd,buf);
    printf("%s",buf);
    return 0;
}
void send_msg(int sockfd,char buf[])
{
    int ret;
    //ret=write(sockfd,buf,strlen(buf));
    ret=send(sockfd,buf,strlen(buf),0);
    if(ret==-1)
    {
       perror("send");
       exit(1);
    }
}
void recv_msg(int sockfd,char buf[])
{
    int ret;
    //ret=read(sockfd,buf,SIZE);
    ret=recv(sockfd,buf,SIZE,0);
    if(ret==0 || ret==-1)  exit(0);
    buf[ret]='\0';   
}
