//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 21:06:39
//

#include <string.h>

#include "buffer.h"

void buffer_init(buffer_t *buffer, heap_t *heap, u32 size, u32 stride)
{
    buffer->parent_heap = heap;
    buffer->stride = stride;
    buffer->mem = heap_alloc(heap, size);
}

void buffer_upload(buffer_t *buffer, const void *data, u64 size)
{
    memcpy(buffer->mem.cpu_addr, data, size);
}

void buffer_free(buffer_t *buffer)
{
    heap_dealloc(buffer->parent_heap, &buffer->mem);
}
