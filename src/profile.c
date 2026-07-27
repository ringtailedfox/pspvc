#include "profile.h"
#include <stddef.h>

const ProfileParams profiles[] = {
    { "1) H264/AVC (16/9) 480x272 vb=384kb ab=64kb", 0, 3, 384, 64 },
    { "2) H264/AVC (16/9) 480x272 vb=512kb ab=128kb", 0, 3, 512, 128 },
    { "3) H264/AVC (4/3) 368x208 vb=384kb ab=64kb", 0, 2, 384, 64 },
    { "4) H264/AVC (4/3) 368x208 vb=512kb ab=128kb", 0, 2, 512, 128 },
    { "5) H264/AVC (4/3) 320x240 vb=384kb ab=64kb", 0, 1, 384, 64 },
    { "6) H264/AVC (4/3) 320x240 vb=512kb ab=128kb", 0, 1, 512, 128 },
    { "7) MPEG4 (4/3) 368x208 vb=384kb ab=64kb",     1, 2, 384, 64 },
    { "8) MPEG4 (4/3) 368x208 vb=512kb ab=128kb",    1, 2, 512, 128 },
    { "9) MPEG4 (4/3) 320x240 vb=384kb ab=64kb",     1, 1, 384, 64 },
    { "10) MPEG4 (4/3) 320x240 vb=512kb ab=128kb",   1, 1, 512, 128 },

    { "11) H264/AVC (16/9) 480x272 vb=768kb ab=64kb",   0, 3, 768, 64 },
    { "12) H264/AVC (16/9) 480x272 vb=1024kb ab=64kb",  0, 3, 1024, 64 },
    { "13) H264/AVC (16/9) 480x272 vb=384kb ab=32kb",   0, 3, 384, 32 },
};

const int profiles_count = sizeof(profiles) / sizeof(profiles[0]);

const char *profile_get_label(int idx) {
    if (idx < 0 || idx >= profiles_count) return NULL;
    return profiles[idx].label;
}

const ProfileParams *profile_get(int idx) {
    if (idx < 0 || idx >= profiles_count) return NULL;
    return &profiles[idx];
}

