#include "config.h"
#include "ui_about.h"
#include <gtk/gtk.h>
#include <libintl.h>
#define _(String) gettext(String)

GtkWidget *create_about_dialog(GtkWindow *parent)
{
    GtkWidget *dialog = gtk_dialog_new_with_buttons(
        _("About PSP Video Converter"),
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        _("_Close"),
        GTK_RESPONSE_CLOSE,
        NULL
    );

    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 300);

    GtkWidget *box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    /* PSP logo */
    gchar *path = g_build_filename(PACKAGE_DATA_DIR, "pixmaps", "psp-console.png", NULL);
    GtkWidget *logo = gtk_image_new_from_file(path);
    g_free(path);
    gtk_widget_set_halign(logo, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(box), logo, FALSE, FALSE, 6);

    /* Scrolled text area */
    GtkWidget *scroller = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroller),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scroller, -1, 200);
    gtk_box_pack_start(GTK_BOX(box), scroller, TRUE, TRUE, 6);

    GtkWidget *textview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textview), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textview), GTK_WRAP_WORD);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));

    const char *about_text =
        _("PlayStation Portable Video Converter v0.4 (C Edition)\n"
          "(http://github.com/ringtailedfox/pspvc)\n\n"
          "Inspired by PSPVC v0.3\n"
          "(http://pspvc.sourceforge.net)\n\n"
          "License: GPL v2\n"
          "http://www.gnu.org/licenses/gpl.txt\n\n"
          "Author: Philippe MAES\n\n"
          "C/GTK3 Edition:\n"
          "RingtailedFox (2026)\n\n"
          "Translations:\n"
          "Philippe MAES (English-French)\n"
          "Luca CALABRO (Italian)\n"
          "Tomasz DOMINIKOWSKI (Polish)\n"
          "Ienooh (Slovenian)\n"
          "Christian Stake (German)\n");

    gtk_text_buffer_set_text(buffer, about_text, -1);

    gtk_container_add(GTK_CONTAINER(scroller), textview);

    gtk_widget_show_all(dialog);
    return dialog;
}
