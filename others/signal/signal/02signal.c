
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void sigfunc(int signo)
{
    printf("a sig send:%d\n",signo);
}

int main(void)
{
    signal(SIGCHLD,sigfunc);
    if(fork()==0)//child
    {
       printf("child start...\n");
       sleep(3);
       printf("child over:%d\n",getpid());
       exit(0);
    }

    wait(NULL);
    return 0;
}
