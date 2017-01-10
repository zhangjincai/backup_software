
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#define      SIZE      1024

void time_out(int signo)
{
   ;
}

int main(void)
{
    char buf[SIZE];
    int  ret;
    struct sigaction newac;

    newac.sa_handler=time_out;
    newac.sa_flags=SA_INTERRUPT;//打断
    sigaction(SIGALRM,&newac,NULL);

    printf("input a info:");
    fflush(stdout);

    alarm(5);
    ret=read(STDIN_FILENO,buf,SIZE);
    if(ret==-1 && errno==EINTR){
       printf("\ninput time out....\n");
       return 1;
    }
    buf[ret]='\0';
    printf("===>%s\n",buf);
    return 0;
}
