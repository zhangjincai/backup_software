
#include <stdio.h>
#include <signal.h>

void say_hello(int signo)
{
   if(signo==SIGINT)
     printf("hello....\n");
}
void func(int signo)
{
   if(signo==2)
     printf("func.....\n");
}
int main(void)
{
    void (*ret)(int);

    printf("set say hello action\n");
    //返回原行为
    ret=signal(SIGINT,say_hello);
    if(ret==SIG_ERR){
      fprintf(stderr,"signal SITINT failed");
      return 1;
    }else if(ret==SIG_DFL){
      printf("以前的行为为默认行为\n");
    }

    getchar();
    printf("set func action\n");
    ret=signal(SIGINT,func);
    if(ret==SIG_ERR)   return 2;//error
    else if(ret==say_hello){
       printf("以前的行为为:say hello\n");
    }

    getchar();
    printf("设置为忽略行为\n");
    signal(SIGINT,SIG_IGN);//忽略此信号

    getchar();
    printf("set default action\n");
    signal(SIGINT,SIG_DFL);
    getchar();
    return 0;
}
