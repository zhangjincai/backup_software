
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <poll.h>

#define     PORT        12530
#define     IP          "192.168.200.250"
#define     MSG_SIZE    1024

struct poll{
   struct pollfd  *ptr; //结构体数组的起始地址
   int    n;//成员个数
};
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void *do_work(void *arg);
int  tail_append(struct poll *h,struct pollfd *data);
void del_array(struct poll *h,int index);

int main(void)
{
    int sfd,newfd,ret;
    struct sockaddr_in  peer;
    socklen_t  len=sizeof(struct sockaddr_in);
    pthread_t  tid;
    struct poll  Head={.ptr=NULL};
    struct pollfd  rfds;

    sfd=init_socket();
    if(sfd<0)   return 1;

    //创建监听与转发数据的线程
    ret=pthread_create(&tid,NULL,do_work,(void *)&Head);
    if(ret!=0)
    {
       fprintf(stderr,"create pthread failed\n");
       return 2;
    }
    //----------------------------------------------
    while(1)
    {
       newfd=accept(sfd,(struct sockaddr *)&peer,&len);
       if(newfd==-1)   continue;
       printf("from:\033[32m%s\033[0m\n",inet_ntoa(peer.sin_addr));
       //追加一个对应的poll数组成员用于监听
       rfds.fd    =newfd;
       rfds.events=POLLIN;

       pthread_mutex_lock(&mutex);
       ret=tail_append(&Head,&rfds);
       if(ret!=0){
         close(newfd);
       }
       pthread_mutex_unlock(&mutex);
    }
    return 0;
}

void *do_work(void *arg)
{
    struct poll *h=(struct poll *)arg;
    char buf[MSG_SIZE],msg[MSG_SIZE];
    struct sockaddr_in  peer;
    socklen_t  len=sizeof(struct sockaddr_in);
    int j,i,ret;
    while(1)
    {
       pthread_mutex_lock(&mutex);
       poll(h->ptr,h->n,100);
       for(i=0;i<h->n;i++)
       {
          if(h->ptr[i].revents & POLLIN)
          {
             //谁发的消息
             getpeername(h->ptr[i].fd,(struct sockaddr *)&peer,&len);
             ret=recv(h->ptr[i].fd,buf,MSG_SIZE,0);
             if(ret==0)
             {
                printf("exit:\033[31m%s\033[0m\n",inet_ntoa(peer.sin_addr));
                del_array(h,i);
                break;
             }
             buf[ret]='\0';
             snprintf(msg,MSG_SIZE,"\033[31m%s\033[0m\n  %s",\
                                    inet_ntoa(peer.sin_addr),buf);
             //转发
             for(j=0;j<h->n;j++)
             {
                send(h->ptr[j].fd,msg,strlen(msg),MSG_NOSIGNAL);
             }
          }
       }
       pthread_mutex_unlock(&mutex);
       sched_yield();//让出cpu高度
    }
    pthread_exit((void *)0);
}

int init_socket(void)
{
    int sfd,ret;
    struct sockaddr_in  addr;
    sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd==-1)
    {
       perror("socket");
       return -1;
    }
    addr.sin_family=AF_INET;
    addr.sin_port  =htons(PORT);
    addr.sin_addr.s_addr=INADDR_ANY;
    memset(addr.sin_zero,0,8);
    ret=bind(sfd,(struct sockaddr *)&addr,sizeof(addr));
    if(ret==-1)
    {
       perror("bind");
       return -2;
    }
    if(listen(sfd,10)==-1)
    {
       perror("listen");
       return -3;
    }
    return sfd;
}


int  tail_append(struct poll *h,struct pollfd *data)
{
    struct pollfd *New;
    New=realloc(h->ptr,(h->n+1)*sizeof(struct pollfd));
    if(New==NULL)   return -1;
    //new data copy to new space tail
    memcpy(New+h->n,data,sizeof(struct pollfd));
    h->ptr=New;
    (h->n)++;
    return 0;
}
void del_array(struct poll *h,int index)
{
    struct pollfd *New;
    memmove(h->ptr+index,h->ptr+(index+1),(h->n-index-1)*sizeof(struct pollfd));
    New=realloc(h->ptr,(h->n-1)*sizeof(struct pollfd));
    if(New==NULL && h->n>1)   return;
    h->ptr=New;
    (h->n)--;
}




















