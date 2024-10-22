//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-22 22:14:02
//

#ifndef BITMAP_H_
#define BITMAP_H_

#include "core.h"

typedef struct bitmap_t {
    u32 width;
    u32 height;
    u8* pixels;
} bitmap_t;

bitmap_t bitmap_load(const char *path);
void bitmap_free(bitmap_t *bitmap);

#endif
