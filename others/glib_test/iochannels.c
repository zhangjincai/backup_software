#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <glib.h>

static gboolean gio_in (GIOChannel *gio, GIOCondition condition, gpointer data)
{
        GIOStatus ret;
        GError *err = NULL;
        gchar *msg;
        gsize len;

        if (condition & G_IO_HUP)
                g_error ("Read end of pipe died!\n");

        ret = g_io_channel_read_line (gio, &msg, &len, NULL, &err);
        if (ret == G_IO_STATUS_ERROR)
                g_error ("Error reading: %s\n", err->message);

        printf ("Read %u bytes: %s\n", len, msg);

        g_free (msg);

        return TRUE;
}

static gboolean gio_out (GIOChannel *gio, GIOCondition condition, gpointer data)
{
        const gchar *msg = "The price of greatness is responsibility.\n";
        GIOStatus ret;
        GError *err = NULL;
        gsize len;

        if (condition & G_IO_HUP)
                g_error ("Write end of pipe died!\n");

        ret = g_io_channel_write_chars (gio, msg, -1, &len, &err);
        if (ret == G_IO_STATUS_ERROR)
                g_error ("Error writing: %s\n", err->message);

        printf ("Wrote %u bytes.\n", len);

        return TRUE;
}

void init_channels (void)
{
        GIOChannel *gio_read, *gio_write;
        int fd[2], ret;

        ret = pipe (fd);
        if (ret == -1)
                g_error ("Creating pipe failed: %s\n", strerror (errno));

        gio_read = g_io_channel_unix_new (fd[0]);
        gio_write = g_io_channel_unix_new (fd[1]);
        if (!gio_read || !gio_write)
                g_error ("Cannot create new GIOChannel!\n");

        if (!g_io_add_watch (gio_read, G_IO_IN | G_IO_HUP, gio_in, NULL))
                g_error ("Cannot add watch on GIOChannel!\n");
        if (!g_io_add_watch (gio_write, G_IO_OUT | G_IO_HUP, gio_out,
NULL))
                g_error ("Cannot add watch on GIOChannel!\n");
}

int main (void)
{
        GMainLoop *loop = g_main_loop_new (NULL, FALSE);
        init_channels ();
        g_main_loop_run (loop); /* Wheee! */
        return 0;
}
