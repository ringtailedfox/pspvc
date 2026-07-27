#ifndef PROFILE_H
#define PROFILE_H

typedef struct {
    const char *label;
    int codec;      // 0 = H.264, 1 = MPEG4
    int aspect;     // 3, 2, 1
    int vbit;       // kbps
    int abit;       // kbps
} ProfileParams;

extern const ProfileParams profiles[];
extern const int profiles_count;

const char *profile_get_label(int idx);
const ProfileParams *profile_get(int idx);

#endif
