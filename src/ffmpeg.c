#include "ui_convert.h"
#include "ffmpeg.h"
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <libintl.h>
#define _(String) gettext(String)



static gboolean on_ffmpeg_output(GIOChannel *source,
                                 GIOCondition condition,
                                 gpointer user_data);
static void on_ffmpeg_finished(GPid pid, gint status, gpointer user_data);

/* Get duration via ffprobe (like Python get_duration_seconds) */
double ffmpeg_get_duration(const char *path)
{
    const char *ffprobe = "/usr/bin/ffprobe";
    gchar *argv[] = {
        (gchar *)ffprobe,
        "-v", "error",
        "-show_entries", "format=duration",
        "-of", "default=noprint_wrappers=1:nokey=1",
        (gchar *)path,
        NULL
    };

    gchar *stdout_data = NULL;
    gchar *stderr_data = NULL;
    gint exit_status = 0;

    if (!g_spawn_sync(NULL, argv, NULL,
                      G_SPAWN_SEARCH_PATH,
                      NULL, NULL,
                      &stdout_data, &stderr_data,
                      &exit_status, NULL)) {
        g_free(stdout_data);
        g_free(stderr_data);
        return 0.0;
    }

    double dur = 0.0;
    if (exit_status == 0 && stdout_data) {
        dur = g_ascii_strtod(stdout_data, NULL);
    }

    g_free(stdout_data);
    g_free(stderr_data);
    return dur;
}

/* Build and start FFmpeg process (like Python on_convert) */
void ffmpeg_start(AppWidgets *app,
                  const char *input,
                  const char *output,
                  const ProfileParams *profile,
                  int volume_percent)
{

    /* Determine output directory */
    char *input_dir = g_path_get_dirname(input);
    char *videos_dir = g_build_filename(g_get_home_dir(), "Videos", NULL);

    /* If input is already in ~/Videos, use that */
    char *out_dir = NULL;
    if (g_strcmp0(input_dir, videos_dir) == 0)
        out_dir = g_strdup(input_dir);
    else
        out_dir = g_strdup(videos_dir);

    g_free(input_dir);
    g_free(videos_dir);

    /* Build full output path */
    char *full_output = g_build_filename(out_dir, output, NULL);
    g_free(out_dir);

    /* Save full output path for THM generation */
    app->full_output_path = g_strdup(full_output);

    /* Store duration for progress calculations */
    app->duration_seconds = ffmpeg_get_duration(input);

    GPtrArray *args = g_ptr_array_new();

    g_ptr_array_add(args, (gpointer)FFMPEG_PATH);
    g_ptr_array_add(args, (gpointer)"-y");
    g_ptr_array_add(args, (gpointer)"-i");
    g_ptr_array_add(args, (gpointer)input);
    g_ptr_array_add(args, (gpointer)"-progress");
    g_ptr_array_add(args, (gpointer)"pipe:1");
    g_ptr_array_add(args, (gpointer)"-nostdin");
    g_ptr_array_add(args, (gpointer)"-stats_period");
    g_ptr_array_add(args, (gpointer)"0.5");

    /* Audio */
    g_ptr_array_add(args, (gpointer)"-c:a");
    g_ptr_array_add(args, (gpointer)"aac");

    gchar *ab_str = g_strdup_printf("%dk", profile->abit);
    g_ptr_array_add(args, (gpointer)"-b:a");
    g_ptr_array_add(args, (gpointer)ab_str);

    double vol = volume_percent / 100.0;
    gchar *vol_str = g_strdup_printf("volume=%f", vol);
    g_ptr_array_add(args, (gpointer)"-af");
    g_ptr_array_add(args, (gpointer)vol_str);

    /* Video codec */
    if (profile->codec == 0) {
        g_ptr_array_add(args, (gpointer)"-c:v");
        g_ptr_array_add(args, (gpointer)"libx264");
    } else {
        g_ptr_array_add(args, (gpointer)"-c:v");
        g_ptr_array_add(args, (gpointer)"mpeg4");
    }

    gchar *vb_str = g_strdup_printf("%dk", profile->vbit);
    g_ptr_array_add(args, (gpointer)"-b:v");
    g_ptr_array_add(args, (gpointer)vb_str);

    /* Sample rate */
    if (profile->codec == 0) {
        g_ptr_array_add(args, (gpointer)"-ar");
        g_ptr_array_add(args, (gpointer)"48000");
    } else {
        g_ptr_array_add(args, (gpointer)"-ar");
        g_ptr_array_add(args, (gpointer)"24000");
    }

    /* Size from aspect */
    const char *size = NULL;
    if (profile->aspect == 3)
        size = "480x272";
    else if (profile->aspect == 2)
        size = "368x208";
    else
        size = "320x240";

    g_ptr_array_add(args, (gpointer)"-s");
    g_ptr_array_add(args, (gpointer)size);

    g_ptr_array_add(args, (gpointer)"-r");
    g_ptr_array_add(args, (gpointer)"30000/1001");

    g_ptr_array_add(args, (gpointer)"-movflags");
    g_ptr_array_add(args, (gpointer)"+faststart");

    /* Title: from entry_title */
    const char *title = gtk_entry_get_text(GTK_ENTRY(app->entry_title));
    if (title && *title) {
        g_ptr_array_add(args, (gpointer)"-title");
        g_ptr_array_add(args, (gpointer)title);
    }

    g_ptr_array_add(args, full_output);
    g_ptr_array_add(args, NULL);

    gchar **argv = (gchar **)g_ptr_array_free(args, FALSE);

    GError *error = NULL;
    gint stdout_fd = -1;
    gint stderr_fd = -1;

    if (!g_spawn_async_with_pipes(NULL,
                                  argv,
                                  NULL,
                                  G_SPAWN_DO_NOT_REAP_CHILD,
                                  NULL, NULL,
                                  &app->ffmpeg_pid,
                                  &stdout_fd,
                                  &stderr_fd,
                                  NULL,
                                  &error)) {
        g_printerr(_("Failed to start ffmpeg: %s\n"), error->message);
        g_error_free(error);

        /* free only the heap strings we allocated */
        g_free(ab_str);
        g_free(vol_str);
        g_free(vb_str);
        g_free(argv);      /* free the array itself, NOT its elements */
        g_free(full_output);
        return;
    }

    /* success path: same deal */
    g_free(ab_str);
    g_free(vol_str);
    g_free(vb_str);
    g_free(argv);
    g_free(full_output);

    /* Watch child exit */
    g_child_watch_add(app->ffmpeg_pid, on_ffmpeg_finished, app);

    /* Watch stdout for progress */
    GIOChannel *channel = g_io_channel_unix_new(stderr_fd);
    g_io_channel_set_encoding(channel, NULL, NULL);
    g_io_channel_set_flags(channel, G_IO_FLAG_NONBLOCK, NULL);

    g_io_add_watch(channel, G_IO_IN | G_IO_HUP, on_ffmpeg_output, app);
}

/* Parse ffmpeg -progress output (like Python on_ffmpeg_output) */
static gboolean on_ffmpeg_output(GIOChannel *source,
                                 GIOCondition condition,
                                 gpointer user_data)
{
    AppWidgets *app = (AppWidgets *)user_data;

    if (condition & G_IO_HUP) {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->progress), 1.0);
        gtk_progress_bar_set_text(GTK_PROGRESS_BAR(app->progress),
                                  _("100% - complete"));
        return FALSE;
    }

    gchar *line = NULL;
    gsize len = 0;
    GIOStatus status = g_io_channel_read_line(source, &line, &len, NULL, NULL);

    if (status == G_IO_STATUS_NORMAL && line) {
        g_strchomp(line);

        if (g_str_has_prefix(line, "out_time=") && app->duration_seconds > 0.0) {
            const char *t = line + strlen("out_time=");
            int h = 0, m = 0;
            double s = 0.0;
            sscanf(t, "%d:%d:%lf", &h, &m, &s);
            double seconds = h * 3600 + m * 60 + s;

            double frac = seconds / app->duration_seconds;
            if (frac < 0.0) frac = 0.0;
            if (frac > 1.0) frac = 1.0;

            int percent = (int)(frac * 100.0);
            gchar *txt = g_strdup_printf(_("%d%% complete"), percent);

            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->progress), frac);
            gtk_progress_bar_set_text(GTK_PROGRESS_BAR(app->progress), txt);

            g_free(txt);
        } else if (g_str_has_prefix(line, "progress=")) {
            const char *status_str = line + strlen("progress=");
            if (g_strcmp0(status_str, "end") == 0) {
                gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(app->progress), 1.0);
                gtk_progress_bar_set_text(GTK_PROGRESS_BAR(app->progress),
                                          _("100% - complete"));
            }
        }
    }

    g_free(line);
    return TRUE;
}

/* Child exit handler */
static void on_ffmpeg_finished(GPid pid, gint status, gpointer user_data)
{
    AppWidgets *app = (AppWidgets *)user_data;

    /* Auto-generate THM on completion */
    char *thm = ffmpeg_generate_thumbnail(app);
    if (thm && g_file_test(thm, G_FILE_TEST_EXISTS)) {
        gtk_image_set_from_file(GTK_IMAGE(app->thumb), thm);
    }
    g_free(thm);

    /* Switch Cancel -> Close */
    convert_window_switch_to_close(app);

    g_spawn_close_pid(pid);
}

/* Manual + auto THM generation (like Python generate_thm) */
char *ffmpeg_generate_thumbnail(AppWidgets *app)
{
    const char *src = gtk_entry_get_text(GTK_ENTRY(app->entry_filename));
    const char *pspname = gtk_entry_get_text(GTK_ENTRY(app->entry_pspname));

    if (!src || !*src || !pspname || !*pspname)
        return NULL;

    /* Build full PSP path in ~/Videos */
    char *videos_dir = g_build_filename(g_get_home_dir(), "Videos", NULL);
    char *psp_full = g_build_filename(videos_dir, pspname, NULL);
    g_free(videos_dir);

    /* Split into directory + basename */
    gchar *dir = g_path_get_dirname(psp_full);
    gchar *base = g_path_get_basename(psp_full);

    gchar *dot = strrchr(base, '.');
    if (dot) *dot = '\0';

    gchar *thm_path = g_build_filename(dir, base, NULL);
    gchar *thm_full = g_strdup_printf("%s.THM", thm_path);

    g_free(thm_path);
    g_free(dir);
    g_free(base);
    g_free(psp_full);

    /* Use selected second */
    int second = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app->spin_second));

    gchar *argv[] = {
        (gchar *)FFMPEG_PATH,
        "-y",
        "-ss", g_strdup_printf("%d", second),
        "-i", (gchar *)src,
        "-vframes", "1",
        "-s", "160x120",
        "-f", "mjpeg",
        thm_full,
        NULL
    };

    gchar *stdout_data = NULL;
    gchar *stderr_data = NULL;
    gint exit_status = 0;

    g_spawn_sync(NULL, argv, NULL,
                 G_SPAWN_SEARCH_PATH,
                 NULL, NULL,
                 &stdout_data, &stderr_data,
                 &exit_status, NULL);

    g_free(stdout_data);
    g_free(stderr_data);
    g_free(argv[3]); /* g_strdup_printf result */

    return thm_full;
}


