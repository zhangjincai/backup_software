#include <glib.h>

void each_callback(gpointer data, gpointer user_data)
{
	g_print("element:%s, user param:%s\n", (gchar*)data, (gchar*)user_data);
}

int main( int argc,
          char *argv[] )
{
  GList *list = NULL;
  list = g_list_append(list, "second");
  list = g_list_prepend(list, "first");

  g_list_foreach(list, each_callback, "user_data");

  GList *second = g_list_find(list, "second");
  g_print("findElement:%s\n", (gchar*)second->data);

  list = g_list_remove(list, "second");

  g_list_foreach(list, each_callback, "user_data");

  return 0;
}
