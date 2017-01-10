#include <string.h> // 或 string.h  
#include <stdlib.h>  
#include <stdio.h>  
  
int main(int argc, const char *argv[]) {  
#if 0
  char* const delim = "/";  
  char str[] = "some/split/string";  
  //char *str = "some/split/string";  
  
  char *token, *cur = str;  
  while (token = strsep(&cur, delim)) {  //&cur:&*cur
    printf("%s\n", token);  
  }  
#endif
#if 0
    char s[] = "hello, world! welcome to china!";  
    char delim[] = " ,!";  
  
    char *token;  
    for(token = strtok(s, delim); token != NULL; token = strtok(NULL, delim)) {  
        printf(token);  
        printf("+");  
    }  
    printf("\n");  
#else
    char source[] = "hello, world! welcome to china!";  
    char delim[] = " ,!";  
  
    char *s = strdup(source);  
    //char *s = source;  
    printf("%s\n",s);
    char *token;  
    for(token = strsep(&s, delim); token != NULL; token = strsep(&s, delim)) {  
        printf(token);  
        printf("+");  
    }  
    printf("\n");
#endif

    return 0;  
} 
