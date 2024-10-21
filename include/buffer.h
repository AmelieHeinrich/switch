//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 21:00:42
//

#ifndef BUFFER_H_
#define BUFFER_H_

#include "heap.h"

typedef struct buffer_t {
    heap_t *parent_heap;
    heap_alloc_t mem;
    u32 stride;
} buffer_t;

void buffer_init(buffer_t *buffer, heap_t *heap, u32 size, u32 stride);
void buffer_upload(buffer_t *buffer, const void *data, u64 size);
void buffer_free(buffer_t *buffer);

#endif
