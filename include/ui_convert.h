#ifndef UI_CONVERT_H
#define UI_CONVERT_H

#include <gtk/gtk.h>
#include "profile.h"
#include "ffmpeg.h"
#include "ui_main.h"   /* if AppWidgets is declared there */

GtkWidget *create_convert_window(AppWidgets *app,
                                 const char *input,
                                 const char *output);

void ffmpeg_start(AppWidgets *app,
                  const char *input,
                  const char *output,
                  const ProfileParams *profile,
                  int volume_percent);

char *ffmpeg_generate_thumbnail(AppWidgets *app);

void convert_window_switch_to_close(AppWidgets *app);

#endif
