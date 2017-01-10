#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "memleak.h"

int main()
{
  void *a,*b,*c;
  char *s;

  dbg_init(10);
  dbg_catch_sigsegv();
  
#if 1
  a = malloc(1000);//a:1000
  b = malloc(30);//30
  a = realloc(a, 150);//a:1150
  c = calloc(90, 3);//c:270
  b = realloc(b, 0);

  malloc(0);
  calloc(0, 10);
  realloc(0, 10);//10

  realloc(a, 0); 

  free(0);

  s = strdup("A string.");//10
  s = strndup(s, 5);//6

  puts(s);

#else
	a = malloc(1111);
	b = malloc(11);

	a = realloc(a,9999);
	
	free(a);
#endif

  dbg_mem_stat();
  dbg_heap_dump("");
  return 0;
}
