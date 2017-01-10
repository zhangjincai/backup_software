
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sigfunc(int signo)
{
    printf("a sig send:%d\n",signo);
    if(signo==SIGFPE){
      printf("除零错误....\n");
      exit(1);
    }else if(signo==SIGSEGV){
      printf("访问非法地址.....\n");
      exit(2);
    }else if(signo==14){
      printf("timeout...\n");
      exit(3);
    }
}

int main(void)
{
    int val;
    int *ptr=NULL;
    signal(SIGFPE,sigfunc);//捕捉行为 信号发生时执行sigfun函数
    signal(SIGSEGV,sigfunc);
    signal(14,sigfunc);

    val=val/0;
    *ptr=100;

    alarm(5);
    printf("======main over========\n");
    getchar();
    return 0;
}
