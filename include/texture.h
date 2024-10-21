//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 15:59:05
//

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "arena.h"

typedef struct texture_t {
    // @note(ame): Internals
    DkImageLayout layout;
    DkImage image;

    // @note(ame): Data
    DkImageFormat format;
    u32 flags;

    u32 width;
    u32 height;

    // @note(ame): Memory footprint
    u32 size;
    u32 alignment;
} texture_t;

#endif
