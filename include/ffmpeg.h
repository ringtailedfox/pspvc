#ifndef FFMPEG_H
#define FFMPEG_H

#include <gtk/gtk.h>
#include "ui_main.h"
#include "profile.h"

#define FFMPEG_PATH "/usr/bin/ffmpeg"

void ffmpeg_start(AppWidgets *app,
                  const char *input,
                  const char *output,
                  const ProfileParams *profile,
                  int volume_percent);

char *ffmpeg_generate_thumbnail(AppWidgets *app);

double ffmpeg_get_duration(const char *path);

#endif
