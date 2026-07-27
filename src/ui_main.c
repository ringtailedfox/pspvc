#include "config.h"
#include <gtk/gtk.h>
#include <libintl.h>
#include <locale.h>
#include <string.h>

#include "ui_main.h"
#include "ui_convert.h"
#include "ui_about.h"
#include "ffmpeg.h"
#include "profile.h"

#define _(String) gettext(String)

/* Tooltips for profiles (raw strings; translated at runtime) */
static const char *TOOLTIPS[] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    "Experimental - High Quality: 768kbps video, 64kbps audio",
    "Experimental - High Quality+: 1024kbps video, 64kbps audio",
    "Experimental - Low Power: 384kbps video, 32kbps audio"
};

static void on_browse_clicked(GtkButton *button, gpointer user_data);
static void on_convert_clicked(GtkButton *button, gpointer user_data);
static void on_quit_clicked(GtkButton *button, gpointer user_data);
static void on_about_clicked(GtkButton *button, gpointer user_data);
static gboolean on_profile_tooltip(GtkWidget *widget,
                                   int x, int y,
                                   gboolean keyboard_mode,
                                   GtkTooltip *tooltip,
                                   gpointer user_data);

/* Main window creation */
GtkWidget *create_main_window(AppWidgets *app)
{
    /* i18n init (main.c already sets locale/bindtextdomain) */

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), _("PSP Video Converter (v0.4 C Edition)"));
    gtk_window_set_default_size(GTK_WINDOW(win), 600, 300);

    app->window = win;

    /* Main grid */
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 6);
    gtk_container_add(GTK_CONTAINER(win), grid);

    /* Window padding */
    gtk_widget_set_margin_start(grid, 12);
    gtk_widget_set_margin_end(grid, 12);
    gtk_widget_set_margin_top(grid, 12);
    gtk_widget_set_margin_bottom(grid, 12);

    /* Top banner */
{
    gchar *path = g_build_filename(PACKAGE_DATA_DIR, "pixmaps", "pspvc_09.png", NULL);
    // g_warning("BANNER PATH: %s", path);  // <-- add this
    GtkWidget *banner = gtk_image_new_from_file(path);
    g_free(path);

    gtk_widget_set_halign(banner, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(banner, TRUE);
    gtk_widget_set_margin_top(banner, 10);
    gtk_widget_set_margin_bottom(banner, 10);

    gtk_grid_attach(GTK_GRID(grid), banner, 0, 0, 4, 1);
}


    /* Input file */
    GtkWidget *lbl_filename = gtk_label_new(_("Filename:"));
    gtk_label_set_xalign(GTK_LABEL(lbl_filename), 0.0);
    gtk_grid_attach(GTK_GRID(grid), lbl_filename, 0, 1, 1, 1);

    /* Input file text area */
    app->entry_filename = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), app->entry_filename, 1, 1, 2, 1);

    /* Browse button */
    GtkWidget *btn_browse = gtk_button_new_with_label(_("Browse"));
    gtk_grid_attach(GTK_GRID(grid), btn_browse, 3, 1, 1, 1);
    g_signal_connect(btn_browse, "clicked",
                     G_CALLBACK(on_browse_clicked), app);

    /* Title */
    GtkWidget *lbl_title = gtk_label_new(_("Title:"));
    gtk_label_set_xalign(GTK_LABEL(lbl_title), 0.0);
    gtk_grid_attach(GTK_GRID(grid), lbl_title, 0, 2, 1, 1);

    /* Title text area */
    app->entry_title = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), app->entry_title, 1, 2, 2, 1);

    /* Profile combo */
    GtkWidget *lbl_profile = gtk_label_new(_("Profile:"));
    gtk_label_set_xalign(GTK_LABEL(lbl_profile), 0.0);
    gtk_grid_attach(GTK_GRID(grid), lbl_profile, 0, 3, 1, 1);

    /* Profile drop-down menu */
    app->combo_profile = gtk_combo_box_text_new();
    for (int i = 0; i < profiles_count; i++) {
        const char *label = profile_get_label(i);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(app->combo_profile),
                                       label ? label : "");
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(app->combo_profile), 0);
    gtk_widget_set_has_tooltip(app->combo_profile, TRUE);
    g_signal_connect(app->combo_profile, "query-tooltip",
                     G_CALLBACK(on_profile_tooltip), app);
    gtk_grid_attach(GTK_GRID(grid), app->combo_profile, 1, 3, 2, 1);

    /* Volume */
    GtkWidget *lbl_volume = gtk_label_new(_("Volume (%):"));
    gtk_label_set_xalign(GTK_LABEL(lbl_volume), 0.0);
    gtk_grid_attach(GTK_GRID(grid), lbl_volume, 0, 4, 1, 1);

    /* Volume spinner selector */
    GtkAdjustment *adj_vol = gtk_adjustment_new(100, 0, 300, 1, 10, 0);
    app->spin_volume = gtk_spin_button_new(adj_vol, 1, 0);
    gtk_grid_attach(GTK_GRID(grid), app->spin_volume, 1, 4, 2, 1);

    /* PSP filename */
    GtkWidget *lbl_pspfilename = gtk_label_new(_("PSP Filename:"));
    gtk_label_set_xalign(GTK_LABEL(lbl_pspfilename), 0.0);
    gtk_grid_attach(GTK_GRID(grid), lbl_pspfilename, 0, 5, 1, 1);

    /* PSP filename text area */
    app->entry_pspname = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), app->entry_pspname, 1, 5, 2, 1);

    /* Buttons: Convert, About */
    /* Convert button */
    GtkWidget *btn_convert = gtk_button_new_with_label(_("Convert"));
    gtk_grid_attach(GTK_GRID(grid), btn_convert, 3, 6, 1, 1);
    g_signal_connect(btn_convert, "clicked",
                     G_CALLBACK(on_convert_clicked), app);

    /* About button */
    GtkWidget *btn_about = gtk_button_new_with_label(_("About"));
    gtk_grid_attach(GTK_GRID(grid), btn_about, 0, 6, 1, 1);
    g_signal_connect(btn_about, "clicked",
                     G_CALLBACK(on_about_clicked), app);

    /* Bottom logo (floating overlay) */
    {
        gchar *path = g_build_filename(PACKAGE_DATA_DIR, "pixmaps", "pspvc_logo.png", NULL);
        GtkWidget *logo = gtk_image_new_from_file(path);
        g_free(path);

        gtk_widget_set_halign(logo, GTK_ALIGN_CENTER);
        gtk_widget_set_margin_top(logo, 0);
        gtk_grid_attach(GTK_GRID(grid), logo, 0, 6, 4, 1);

    }

    gtk_widget_show_all(win);
    return win;
}

/* Browse for input file and suggest PSP filename */
static void on_browse_clicked(GtkButton *button, gpointer user_data)
{
    AppWidgets *app = (AppWidgets *)user_data;

    GtkWidget *dlg = gtk_file_chooser_dialog_new(
        _("Select input video"),
        GTK_WINDOW(app->window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        _("_Cancel"), GTK_RESPONSE_CANCEL,
        _("_Open"), GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dlg)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlg));
        gtk_entry_set_text(GTK_ENTRY(app->entry_filename), filename);

        /* Suggest title from basename */
        char *base = g_path_get_basename(filename);
        char *dot = strrchr(base, '.');
        if (dot) *dot = '\0';
        gtk_entry_set_text(GTK_ENTRY(app->entry_title), base);

        /* Suggest PSP filename using next number */
        char *dir = g_path_get_dirname(filename);
        const char *prefix = "MAQ";
        int next = find_next_psp_number(dir, prefix);
        char *pspname = g_strdup_printf("%s%05d.MP4", prefix, next);
        gtk_entry_set_text(GTK_ENTRY(app->entry_pspname), pspname);

        g_free(pspname);
        g_free(dir);
        g_free(base);
        g_free(filename);
    }

    gtk_widget_destroy(dlg);
}

/* Convert button -> open convert window and start ffmpeg */
static void on_convert_clicked(GtkButton *button, gpointer user_data)
{
    AppWidgets *app = (AppWidgets *)user_data;

    const char *input = gtk_entry_get_text(GTK_ENTRY(app->entry_filename));
    const char *psp   = gtk_entry_get_text(GTK_ENTRY(app->entry_pspname));

    if (!input || !*input || !psp || !*psp)
        return;

    int idx = gtk_combo_box_get_active(GTK_COMBO_BOX(app->combo_profile));
    const ProfileParams *prof = profile_get(idx);
    if (!prof)
        return;

    int volume = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(app->spin_volume));

    /* Create convert window */
    create_convert_window(app, input, psp);

    /* Start ffmpeg */
    ffmpeg_start(app, input, psp, prof, volume);
}

/* About dialog */
static void on_about_clicked(GtkButton *button, gpointer user_data)
{
    AppWidgets *app = (AppWidgets *)user_data;

    GtkWidget *dlg = create_about_dialog(GTK_WINDOW(app->window));
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
}

/* Profile tooltip handler */
static gboolean on_profile_tooltip(GtkWidget *widget,
                                   int x, int y,
                                   gboolean keyboard_mode,
                                   GtkTooltip *tooltip,
                                   gpointer user_data)
{
    AppWidgets *app = (AppWidgets *)user_data;
    int idx = gtk_combo_box_get_active(GTK_COMBO_BOX(app->combo_profile));

    if (idx >= 0 && idx < (int)(sizeof(TOOLTIPS) / sizeof(TOOLTIPS[0])) &&
        TOOLTIPS[idx] != NULL) {
        gtk_tooltip_set_text(tooltip, _(TOOLTIPS[idx]));
        return TRUE;
    }

    return FALSE;
}
