//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 16:09:38
//

// Thank you to our lord and saviour Sebastian Aaltonen for providing us with this gem of an allocator
// https://github.com/sebbbi/OffsetAllocator

#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <switch.h>

#include "core.h"

#define NUM_TOP_BINS 32
#define BINS_PER_LEAF 8
#define TOP_BINS_INDEX_SHIFT 3
#define LEAF_BINS_INDEX_MASK 0x7
#define NUM_LEAF_BINS NUM_TOP_BINS * BINS_PER_LEAF
#define ALLOCATION_NO_SPACE 0xffffffff

typedef struct allocation_t {
    u32 offset;
    u32 metadata;
} allocation_t;

typedef struct allocator_node_t {
    u32 data_offset;
    u32 data_size;
    u32 bin_list_prev;
    u32 bin_list_next;
    u32 neighbor_prev;
    u32 neighbor_next;
    b8 used;
} allocator_node_t;

typedef struct allocator_t {
    u32 size;
    u32 max_allocs;
    u32 free_storage;

    u32 used_bins_top;
    u32 used_bins[NUM_TOP_BINS];
    u32 bin_indices[NUM_LEAF_BINS];

    allocator_node_t* nodes;
    u32* free_nodes;
    u32 free_offset;
} allocator_t;

void allocator_init(allocator_t *allocator, u32 size);
void allocator_free(allocator_t *allocator);
void allocator_reset(allocator_t *allocator);

allocator_node_t allocator_new(u32 size);
void allocator_delete(allocator_node_t *node);

#endif
