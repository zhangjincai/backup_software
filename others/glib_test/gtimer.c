#include <glib.h>

GTimer *timer;

void each_callback(gpointer data, gpointer user_data)
{
	g_print("element:%s, user param:%s\n", (gchar*)data, (gchar*)user_data);
}

int main( int argc,char *argv[] )
{
  int i=0;
  GList *list = NULL;
  gulong seconds;
  timer = g_timer_new();

  list = g_list_append(list, "second");
  list = g_list_prepend(list, "first");

  g_timer_start(timer);
  g_list_foreach(list, each_callback, "user_data");
  g_timer_stop(timer);

  g_timer_elapsed(timer, &seconds);

  g_print("use %ld ms\n", seconds);

  //g_timer_continue(timer);
  g_timer_reset(timer);
  g_timer_start(timer);
  for(i; i<=1000; i++)
  {
	  g_print("%d", i);
  }
  g_timer_stop(timer);

  g_timer_elapsed(timer, &seconds);
  g_print("\nuse %ld ms\n", seconds);

  g_timer_destroy(timer);
  return 0;
}
