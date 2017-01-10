
#include <stdio.h>

struct tt{
    unsigned int a:2; //该成员占其中2个bit位
    unsigned int b:4;
    int c:5;
    int d:10;
    int e:26;
};

int main(void)
{
     struct tt temp;

     printf("size:%d %d\n",sizeof(struct tt),sizeof(temp));

     //&temp.a; 无法取得位段 ‘a’ 的地址
     temp.a=1;
     temp.b=15;
     printf("a:%d b:%d\n",temp.a,temp.b);

     temp.b=0;
     temp.a=4;
     printf("a:%d b:%d\n",temp.a,temp.b);
     return 0;
}
