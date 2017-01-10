#include "glib.h"
#include <stdio.h>

static GMutex *mutex = NULL;  //线程互斥量

void push(GAsyncQueue *queue)  //放入数据
{
    gint i;
   
    for(i = 1; i < 11; i++) {
        g_mutex_lock(mutex);
        g_async_queue_push(queue,(gpointer)i);
        g_mutex_unlock(mutex);
    }
}
void pop(GAsyncQueue *queue)  //取出数据
{
    gint i;
    gint data;
   
    for(i = 1; i < 11; i++) {
        g_mutex_lock(mutex);
        data = (gint)g_async_queue_try_pop(queue);
        g_print("Got %d!\n",data);
        g_mutex_unlock(mutex);
    }
}

int main()
{
	fprintf(stderr,"glib version %d.%d.%d\n", glib_major_version,glib_minor_version,glib_micro_version);

    GAsyncQueue *queue;
   
    if(!g_thread_supported())
        g_thread_init(NULL);
    //初始化线程系统
    mutex = g_mutex_new();  //创建一个新的线程互斥量
    queue = g_async_queue_new();
   
    g_thread_create(push, queue, TRUE, NULL);
    g_thread_create(pop, queue, TRUE, NULL);
   
    g_mutex_free(mutex);
}
