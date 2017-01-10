#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int main()
{

        struct timeval tpstart;
        int n = 100;
        int nn = 0;
        for (nn=0;nn<300;nn++)
        {
                gettimeofday(&tpstart,NULL);
                srand(tpstart.tv_usec);
                //srand( (unsigned)time( NULL ) );
                n = (1+(int) (300.0*rand()/(RAND_MAX+1.0)));
		//n = rand();
                printf("%d \n",n);
#if 0
                if (n<=100)
                {
                        printf("%d  AAAA\n",  n);
                }
                else if (n <= 200)
                {
                        printf("%d  BBBB\n", n);
                }
                else if (n<=300)
                {
                        printf("%d  CCCC\n", n);
                }
                else
                {
                        printf("Error\n");
                }
                sleep(1);
#endif
        }
        return 0;
}
