#include<glib.h> 
#include <stdio.h>

GMainLoop* loop;

gint counter = 20;
gboolean callback(gpointer arg)
{
    g_print(".");
    if(--counter ==0){
        g_print("\n");
        //退出循环
        g_main_loop_quit(loop);
        //注销定时器
        return FALSE;
    }
    //定时器继续运行
    return TRUE;
}

int main(int argc, char* argv[])
{
    fprintf(stderr,"glib version %d.%d.%d\n", glib_major_version,glib_minor_version,glib_micro_version);

    if(g_thread_supported() == 0)
        g_thread_init(NULL);

    g_print("g_main_loop_new\n");
    loop = g_main_loop_new(NULL, FALSE);

    //增加一个定时器，100毫秒运行一次callback
    g_timeout_add(100,callback,NULL);

    g_print("g_main_loop_run\n");
    g_main_loop_run(loop);

    g_print("g_main_loop_unref\n");
    g_main_loop_unref(loop);

    return 0;
}
