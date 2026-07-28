#include "config.h"
#include "ui_convert.h"
#include "ffmpeg.h"
#include <gtk/gtk.h>
#include <string.h>
#include <libintl.h>
#define _(String) gettext(String)

static void on_cancel_clicked(GtkButton *button, gpointer user_data);
static void on_generate_thumbnail(GtkButton *button, gpointer user_data);

GtkWidget *create_convert_window(AppWidgets *app,
                                 const char *input,
                                 const char *output)
{
    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), _("Converting..."));
    gtk_window_set_transient_for(GTK_WINDOW(win), GTK_WINDOW(app->window));
    gtk_container_set_border_width(GTK_CONTAINER(win), 8);

    app->convert_window = win;

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_container_add(GTK_CONTAINER(win), grid);

    /* Labels */
    gtk_grid_attach(GTK_GRID(grid),
                    gtk_label_new(_("Converting:")), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
                    gtk_label_new(input), 1, 0, 2, 1);

    gtk_grid_attach(GTK_GRID(grid),
                    gtk_label_new(_("To:")), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid),
                    gtk_label_new(output), 1, 1, 2, 1);

    /* Progress bar */
    app->progress = gtk_progress_bar_new();
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(app->progress), TRUE);
    gtk_grid_attach(GTK_GRID(grid), app->progress, 0, 2, 3, 1);

    /* Thumbnail placeholder */
    gchar *path = g_build_filename(PKGDATADIR, "pixmaps", "pspvc_thm.png", NULL);
    app->thumb = gtk_image_new_from_file(path);
    g_free(path);
    gtk_grid_attach(GTK_GRID(grid), app->thumb, 0, 3, 1, 2);

    /* THM second selector */
    gtk_grid_attach(GTK_GRID(grid),
                    gtk_label_new(_("From second:")), 1, 3, 1, 1);

    GtkAdjustment *adj = gtk_adjustment_new(1, 0, 3600, 1, 10, 0);
    app->spin_second = gtk_spin_button_new(adj, 1, 0);
    gtk_grid_attach(GTK_GRID(grid), app->spin_second, 2, 3, 1, 1);

    /* Generate THM button */
    GtkWidget *btn_thm = gtk_button_new_with_label(_("Generate Thumbnail"));
    gtk_grid_attach(GTK_GRID(grid), btn_thm, 1, 4, 1, 1);
    g_signal_connect(btn_thm, "clicked",
                     G_CALLBACK(on_generate_thumbnail), app);

    /* Cancel button */
    app->cancel_button = gtk_button_new_with_label(_("Cancel"));
    gtk_grid_attach(GTK_GRID(grid), app->cancel_button, 2, 4, 1, 1);
    g_signal_connect(app->cancel_button, "clicked",
                     G_CALLBACK(on_cancel_clicked), app);

    gtk_widget_show_all(win);
    return win;
}

static void on_close_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *win = GTK_WIDGET(user_data);
    gtk_widget_destroy(win);
}

void convert_window_switch_to_close(AppWidgets *app)
{
    gtk_button_set_label(GTK_BUTTON(app->cancel_button), _("Close"));

    g_signal_handlers_disconnect_by_func(app->cancel_button,
                                         on_cancel_clicked, app);

    /* Cleanup full output path, to prevent a small memory leak after closing */
    if (app->full_output_path) {
        g_free(app->full_output_path);
        app->full_output_path = NULL;
    }

    g_signal_connect(app->cancel_button, "clicked",
                     G_CALLBACK(on_close_clicked), app->convert_window);
}

/* Cancel -> Close logic */
static void on_cancel_clicked(GtkButton *button, gpointer user_data)
{
    AppWidgets *app = (AppWidgets *)user_data;

    if (app->ffmpeg_pid > 0) {
        /* FFmpeg still running -> terminate */
        kill(app->ffmpeg_pid, SIGTERM);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(app->progress),
                                  _("0% - stopped"));
    }

    /* Switch Cancel -> Close */
    gtk_button_set_label(GTK_BUTTON(app->cancel_button), _("Close"));

    g_signal_handlers_disconnect_by_func(app->cancel_button,
                                         on_cancel_clicked, app);

    /* Cleanup full output path, to prevent a small memory leak after closing */
    if (app->full_output_path) {
        g_free(app->full_output_path);
        app->full_output_path = NULL;
    }

    g_signal_connect(app->cancel_button, "clicked",
                     G_CALLBACK(on_close_clicked), app->convert_window);
}

/* Manual THM generation */
static void on_generate_thumbnail(GtkButton *button, gpointer user_data)
{
    AppWidgets *app = (AppWidgets *)user_data;

    /* ffmpeg_generate_thumbnail() will be implemented in ffmpeg.c */
    char *thm = ffmpeg_generate_thumbnail(app);

    if (thm && g_file_test(thm, G_FILE_TEST_EXISTS)) {
        gtk_image_set_from_file(GTK_IMAGE(app->thumb), thm);
    }

    g_free(thm);
}

