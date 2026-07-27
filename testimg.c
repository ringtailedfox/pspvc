#include <gtk/gtk.h>

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);
    GtkWidget *w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *i = gtk_image_new_from_file("/usr/local/share/pspvc/pixmaps/pspvc_09.png");
    gtk_container_add(GTK_CONTAINER(w), i);
    gtk_widget_show_all(w);
    gtk_main();
    return 0;
}
