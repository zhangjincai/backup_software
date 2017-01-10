#include <string.h>

int main(void)
{
  char *s="Golden Global View";
  char *r="new";
  int n;
  
  //clrscr();
  
  n=strcspn(s,r);
  printf("The first char both in s1 and s2 is: %c\n",s[n]);
  
  getchar();
  return 0;
}
