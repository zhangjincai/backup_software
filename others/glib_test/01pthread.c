#include <stdio.h>
#include <glib.h>
#include <pthread.h>
#include <string.h>

#define MAX_THREAD_NUM 10

/** 全局锁*/
pthread_mutex_t g_lock;

/**
 * @brief 线程池回调函数，线程执行的主体函数
 */
void 
show_result(gpointer str, gpointer data)
{

  pthread_mutex_lock(&g_lock);
  printf("The result is : %s\n", str);

  pthread_mutex_unlock(&g_lock);
}

void 
main()
{
  char *str = "thread_pool example";
  int count = 0;
  int g_thread_num = 0;
  GThreadPool *thread_pool;

 /**

   * 初始化互斥锁，NULL表示使用默认的快速互斥锁。

   */
  pthread_mutex_init(&g_lock, NULL);

  g_thread_init(NULL);

  thread_pool = g_thread_pool_new(show_result, NULL, MAX_THREAD_NUM, TRUE, NULL);

  if (thread_pool == NULL)
  {
    printf("g_thread_pool_new failed!\n");
    exit(0);
  }
  /**
   * 调用20次线程池
   * 实际线程池中最多有10个线程同时运行
   */
  for (count; count < 20; count++)
  {
    g_thread_pool_push(thread_pool, (gpointer)str, NULL);
  } 

  g_thread_num =  g_thread_pool_get_max_threads(thread_pool);

  printf("Max number in the pthread pool: %d\n", g_thread_num);
  g_thread_pool_free(thread_pool, 0, 1);

  return;
}
