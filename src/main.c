#include "config.h"
#include <gtk/gtk.h>
#include "ui_main.h"
#include <libintl.h>
#include <locale.h>

#define _(String) gettext(String)

int main(int argc, char *argv[])
{
    /* Initialize locale + gettext */
    setlocale(LC_ALL, "");
    bindtextdomain("pspvc", LOCALEDIR);
    bind_textdomain_codeset("pspvc", "UTF-8");
    textdomain("pspvc");

    setenv("GDK_PIXBUF_MODULEDIR", "/usr/lib/aarch64-linux-gnu/gdk-pixbuf-2.0/2.10.0/loaders", 1);
    setenv("GDK_PIXBUF_MODULE_FILE", "/usr/lib/aarch64-linux-gnu/gdk-pixbuf-2.0/2.10.0/loaders.cache", 1);

    gtk_init(&argc, &argv);

    AppWidgets app = {0};
    GtkWidget *win = create_main_window(&app);

    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(win);
    gtk_main();
    return 0;
}
