
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <strings.h> //for bzero
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define     PORT        4444
#define     IP          "192.168.200.250"
#define     MAX_WAIT    10
#define     SIZE        1024

int init_socket(void);

void *do_work(void *arg);

int main(void)
{
    int sfd,newfd,ret;
    pthread_t tid;
    struct sockaddr_in  peer;
    socklen_t   len=sizeof(struct sockaddr_in);

    signal(SIGPIPE,SIG_IGN);

    sfd=init_socket();
    if(sfd<0)
    {
       fprintf(stderr,"init socket failed\n");
       return 1;
    }

    while(1)
    {
        //newfd=accept(sfd,NULL,NULL);
        newfd=accept(sfd,(void *)&peer,&len);
        if(newfd==-1)  continue;
        printf("\033[31mfrom:%s\033[0m\n",inet_ntoa(peer.sin_addr));

        //产子被调线程进行数据分发
        ret=pthread_create(&tid,NULL,do_work,(void *)newfd);
        if(ret!=0)
        {
           fprintf(stderr,"create pthread failed\n");
           close(newfd);
        }
    }
    return 0;
}
int init_socket(void)
{
    int temp,ret;
    struct sockaddr_in addr;
    temp=socket(PF_INET,SOCK_STREAM,0);
    if(temp==-1){
      perror("socket");
      return -1;
    }
    addr.sin_family=AF_INET;
    addr.sin_port  =htons(PORT);
    //addr.sin_addr.s_addr=inet_addr(IP);
    addr.sin_addr.s_addr=INADDR_ANY;
    bzero(addr.sin_zero,8);
    ret=bind(temp,(struct sockaddr *)&addr,sizeof(addr));
    if(ret==-1)
    {
       perror("bind");
       return -2;
    }
    if(listen(temp,MAX_WAIT)==-1)
    {
       perror("listen");
       return -3;
    }
    return temp;
}

void *do_work(void *arg)
{
    int i,ret,newfd=(int)arg;
    char buf[SIZE];
    pthread_detach(pthread_self());

    for(i=0;i<10;i++)
    {
       snprintf(buf,SIZE,"%dth Hello Cilent",i);
       ret=write(newfd,buf,strlen(buf));
       if(ret==-1)
       {
          perror("write");
          pthread_exit((void *)1);
       }
       sleep(1);
    }
    shutdown(newfd,SHUT_RDWR);
    pthread_exit((void *)0);
}
