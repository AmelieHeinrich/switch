//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 19:07:45
//

#ifndef HEAP_H_
#define HEAP_H_

#include <deko3d.h>

#include "allocator.h"

#define KILOBYTES(b) b * 1024
#define MEGABYTES(b) KILOBYTES(b) * 1024
#define GIGABYTES(b) MEGABYTES(b) * 1024
#define ALIGN(s, a) ((s + a - 1) &~ (a - 1))

// @note(ame): Wrapper for a GPU allocator
typedef struct heap_alloc_t {
    allocation_t internal_allocation;
    b8 valid;

    u32 offset;
    u32 size;

    void* cpu_addr;
    DkGpuAddr gpu_addr;
} heap_alloc_t;

typedef struct heap_t {
    allocator_t allocator;
    u32 flags;

    DkMemBlock block;
} heap_t;

void heap_init(heap_t *heap, u64 size, u32 flags, DkDevice device);
heap_alloc_t heap_alloc(heap_t *heap, u32 size);
void heap_dealloc(heap_t *heap, heap_alloc_t *alloc);
void heap_free(heap_t *heap);

#endif
