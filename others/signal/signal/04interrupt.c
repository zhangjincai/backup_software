
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#define      SIZE      1024

void sigfunc(int signo)
{
    printf("a sig send\n");
    sleep(2);
    printf("a sig over\n");
}

int main(void)
{
    char buf[SIZE];
    int  ret;

//    signal(SIGINT,sigfunc);//is error
    struct sigaction newac,old;
    newac.sa_handler=sigfunc;
    //newac.sa_flags=SA_RESTART;//打断的系统调用实现重启
    newac.sa_flags=SA_INTERRUPT;//打断
    //sigaction(SIGINT,&newac,NULL);
    sigaction(SIGINT,&newac,&old);

    printf("input a info:");
    fflush(stdout);

    ret=read(STDIN_FILENO,buf,SIZE);
    if(ret==-1 && errno==EINTR){
       printf("system call interrupted by signal\n");
       return 1;
    }
    buf[ret]='\0';
    printf("===>%s\n",buf);
    return 0;
}
