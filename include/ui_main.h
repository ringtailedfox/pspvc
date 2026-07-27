#ifndef UI_MAIN_H
#define UI_MAIN_H

#include <gtk/gtk.h>
#include "profile.h"

int find_next_psp_number(const char *directory, const char *prefix);


typedef struct {
    GtkWidget *window;
    GtkWidget *entry_filename;
    GtkWidget *entry_title;
    GtkWidget *combo_profile;
    GtkWidget *spin_volume;
    GtkWidget *entry_pspname;

    GtkWidget *convert_window;
    GtkWidget *progress;
    GtkWidget *label_percent;
    GtkWidget *thumb;
    GtkWidget *cancel_button;

    GtkWidget *spin_second;
    GPid ffmpeg_pid;
    double duration_seconds;
    char *full_output_path;
} AppWidgets;

GtkWidget *create_main_window(AppWidgets *app);

#endif
