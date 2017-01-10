
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigfunc(int signo)
{
    printf("====a sig send:%d===\n",signo);
    sleep(5);
    printf("====sig over [%d]=====\n",signo);
}

int main(void)
{
   int ret;
   printf("====main process pid:%d====\n",getpid());
   signal(SIGINT,sigfunc);
   signal(SIGALRM,sigfunc);

   alarm(3);
   ret=pause();
   if(ret==-1)
     perror("pause");
   return 0;
}
