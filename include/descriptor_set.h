//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-23 00:01:45
//

#ifndef DESCRIPTOR_SET_H_
#define DESCRIPTOR_SET_H_

#include "core.h"
#include "heap.h"

typedef struct descriptor_set_t {
    heap_t *parent_heap;
    heap_alloc_t mem;
    u32 descriptor_count;
} descriptor_set_t;

descriptor_set_t descriptor_set_init(heap_t *heap, u32 descriptor_count);
void descriptor_set_free(descriptor_set_t *set);

#endif
