//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 15:59:05
//

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "heap.h"

typedef struct texture_t {
    // @note(ame): Internals
    DkImageLayout layout;
    DkImage image;
    DkImageView image_view;
    DkImageDescriptor descriptor;

    // @note(ame): Data
    DkImageFormat format;
    u32 flags;

    u32 width;
    u32 height;

    // @note(ame): Memory footprint
    u32 size;
    u32 alignment;

    // @note(ame): Parent heaps and stuff
    heap_t *parent_heap;
    heap_alloc_t alloc;
} texture_t;

void texture_init(texture_t *texture, u32 width, u32 height, DkImageFormat format, heap_t *heap, DkDevice device, u32 flags);
void texture_free(texture_t *texture);

#endif
