#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include "ui_main.h"

/* Find next PSP number: scans directory for M4Vxxxxx.MP4 */
int find_next_psp_number(const char *directory, const char *prefix)
{
    int max_num = 0;

    GDir *dir = g_dir_open(directory, 0, NULL);
    if (!dir)
        return 1;

    const char *name;
    while ((name = g_dir_read_name(dir)) != NULL) {

        /* Must start with prefix */
        if (!g_str_has_prefix(name, prefix))
            continue;

        /* Must end with .MP4 */
        if (!g_str_has_suffix(name, ".MP4"))
            continue;

        /* Extract numeric part: prefix + 5 digits */
        const char *numstr = name + strlen(prefix);

        /* Ensure exactly 5 digits */
        if (strlen(numstr) < 9)  /* "00001.MP4" = 9 chars */
            continue;

        char digits[6];
        memcpy(digits, numstr, 5);
        digits[5] = '\0';

        int n = atoi(digits);
        if (n > max_num)
            max_num = n;
    }

    g_dir_close(dir);

    return max_num + 1;
}
