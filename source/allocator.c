//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 17:06:50
//

#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "util.h"

#if 0
#define MANTISSA_BITS 3
#define MANTISSA_VALUE (1 << MANTISSA_BITS)
#define MANTISSA_MASK (MANTISSA_VALUE - 1)

u32 lzcnt_nonzero(u32 v)
{
    return __builtin_clz(v);
}

u32 tzcnt_nonzero(u32 v)
{
    return __builtin_ctz(v);
}

u32 uint_to_float_roundup(u32 size)
{
    u32 exp = 0;
    u32 mantissa = 0;

    if (size < MANTISSA_VALUE) {
        mantissa = size;
    } else {
        u32 leading_zeroes = lzcnt_nonzero(size);
        u32 highest_set_bit = 31 - leading_zeroes;

        u32 mantissa_start_bit = highest_set_bit - MANTISSA_BITS;
        exp = mantissa_start_bit + 1;
        mantissa = (size >> mantissa_start_bit) & MANTISSA_MASK;

        u32 low_bits_mask = (1 << mantissa_start_bit) - 1;

        // round up
        if ((size & low_bits_mask) != 0)
            mantissa++;
    }

    return (exp << MANTISSA_BITS) + mantissa;
}

u32 uint_to_float_round_down(u32 size)
{
    u32 exp = 0;
    u32 mantissa = 0;

    if (size < MANTISSA_VALUE) {
        mantissa = size;
    } else {
        u32 leading_zeroes = lzcnt_nonzero(size);
        u32 highest_set_bit = 31 - leading_zeroes;

        u32 mantissa_start_bit = highest_set_bit - MANTISSA_BITS;
        exp = mantissa_start_bit + 1;
        mantissa = (size >> mantissa_start_bit) & MANTISSA_MASK;
    }

    return (exp << MANTISSA_BITS) | mantissa;
}

u32 float_to_uint(u32 float_val)
{
    u32 exponent = float_val >> MANTISSA_BITS;
    u32 mantissa = float_val & MANTISSA_MASK;
    if (exponent == 0) {
        return mantissa;
    } else {
        return (mantissa | MANTISSA_VALUE) << (exponent - 1);
    }
}

u32 find_lowest_set_bit_after(u32 bitmask, u32 start_bit_index)
{
    u32 mask_before_start_index = (1 << start_bit_index) - 1;
    u32 mask_after_start_index = ~mask_before_start_index;
    u32 bits_after = bitmask & mask_after_start_index;
    if (bits_after == 0) return ALLOCATION_NO_SPACE;
    return tzcnt_nonzero(bits_after);
}

u32 allocator_insert_node_into_bin(allocator_t *allocator, u32 size, u32 data_offset)
{
    u32 bin_index = uint_to_float_round_down(size);

    u32 top_bin_index = bin_index >> TOP_BINS_INDEX_SHIFT;
    u32 leaf_bin_index = bin_index & LEAF_BINS_INDEX_MASK;

    if (allocator->bin_indices[bin_index] == ALLOCATION_NO_SPACE) {
        allocator->used_bins[top_bin_index] |= 1 << leaf_bin_index;
        allocator->used_bins_top |= 1 << top_bin_index;
    }

    u32 top_node_index = allocator->bin_indices[bin_index];
    u32 node_index = allocator->free_nodes[allocator->free_offset--];

    allocator->nodes[node_index].data_offset = data_offset;
    allocator->nodes[node_index].data_size = size;
    allocator->nodes[node_index].bin_list_next = top_node_index;
    if (top_node_index != ALLOCATION_NO_SPACE) allocator->nodes[node_index].bin_list_prev = node_index;
    allocator->bin_indices[bin_index] = node_index;

    allocator->free_storage += size;
    return node_index;
}

void allocator_remove_node_from_bin(allocator_t *allocator, u32 node_index)
{
    allocator_node_t *node = &allocator->nodes[node_index];

    if (node->bin_list_prev != ALLOCATION_NO_SPACE) {
        allocator->nodes[node->bin_list_prev].bin_list_prev = node->bin_list_next;
        if (node->bin_list_next != ALLOCATION_NO_SPACE) allocator->nodes[node->bin_list_next].bin_list_prev = node->bin_list_prev;
    } else {
        u32 bin_index = uint_to_float_round_down(node->data_size);
        u32 top_bin_index = bin_index >> TOP_BINS_INDEX_SHIFT;
        u32 leaf_bin_index = bin_index & LEAF_BINS_INDEX_MASK;

        allocator->bin_indices[bin_index] = node->bin_list_next;
        if (node->bin_list_next != ALLOCATION_NO_SPACE) allocator->nodes[node->bin_list_next].bin_list_prev = ALLOCATION_NO_SPACE;

        if (allocator->bin_indices[bin_index] == ALLOCATION_NO_SPACE) {
            allocator->used_bins[bin_index] &= ~(1 << leaf_bin_index);

            if (allocator->used_bins[top_bin_index] == 0) {
                allocator->used_bins_top &= ~(1 << top_bin_index);
            }
        }
    }

    allocator->free_nodes[++allocator->free_offset] = node_index;
    allocator->free_storage -= node->data_size;
}

void allocator_init(allocator_t *allocator, u32 size)
{
    memset(allocator, 0, sizeof(allocator_t));
    allocator->size = size;
    allocator->max_allocs = 128 * 1024;
    allocator->nodes = NULL;
    allocator->free_nodes = NULL;

    allocator_reset(allocator);
}

void allocator_free(allocator_t *allocator)
{
    free(allocator->nodes);
    free(allocator->free_nodes);
}

void allocator_reset(allocator_t *allocator)
{
    allocator->free_storage = 0;
    allocator->used_bins_top = 0;
    allocator->free_offset = allocator->max_allocs - 1;

    for (u32 i = 0; i < NUM_TOP_BINS; i++)
        allocator->used_bins[i] = 0;

    for (u32 i = 0; i < NUM_LEAF_BINS; i++)
        allocator->bin_indices[i] = ALLOCATION_NO_SPACE;

    if (allocator->nodes) free(allocator->nodes);
    if (allocator->free_nodes) free(allocator->free_nodes);

    allocator->nodes = malloc(sizeof(allocator_node_t) * allocator->max_allocs);
    allocator->free_nodes = malloc(sizeof(u32) * allocator->max_allocs);

    for (u32 i = 0; i < allocator->max_allocs; i++) {
        allocator->free_nodes[i] = allocator->max_allocs - i - 1;

        allocator->nodes[i].data_offset = 0;
        allocator->nodes[i].data_size = 0;
        allocator->nodes[i].bin_list_prev = ALLOCATION_NO_SPACE;
        allocator->nodes[i].bin_list_next = ALLOCATION_NO_SPACE;
        allocator->nodes[i].neighbor_prev = ALLOCATION_NO_SPACE;
        allocator->nodes[i].neighbor_next = ALLOCATION_NO_SPACE;
        allocator->nodes[i].used = false;
    }

    allocator_insert_node_into_bin(allocator, allocator->size, 0);
}

allocation_t allocator_new(allocator_t *allocator, u32 size)
{
    if (allocator->free_offset == 0) {
        return (allocation_t){
            .offset = ALLOCATION_NO_SPACE,
            .metadata = ALLOCATION_NO_SPACE
        };
    }

    u32 min_bin_index = uint_to_float_roundup(size);

    u32 min_top_bin_index = min_bin_index >> TOP_BINS_INDEX_SHIFT;
    u32 min_leaf_bin_index = min_bin_index & LEAF_BINS_INDEX_MASK;

    u32 top_bin_index = min_top_bin_index;
    u32 leaf_bin_index = ALLOCATION_NO_SPACE;

    if (allocator->used_bins_top & (1 << top_bin_index)) {
        leaf_bin_index = find_lowest_set_bit_after(allocator->used_bins[top_bin_index], min_leaf_bin_index);
    }

    if (leaf_bin_index == ALLOCATION_NO_SPACE) {
        top_bin_index = find_lowest_set_bit_after(allocator->used_bins_top, min_top_bin_index + 1);

        if (top_bin_index == ALLOCATION_NO_SPACE) {
            return (allocation_t){
                .offset = ALLOCATION_NO_SPACE,
                .metadata = ALLOCATION_NO_SPACE
            };
        }

        leaf_bin_index = tzcnt_nonzero(allocator->used_bins[top_bin_index]);
    }

    u32 bin_index = (top_bin_index << TOP_BINS_INDEX_SHIFT) | leaf_bin_index;

    u32 node_index = allocator->bin_indices[bin_index];
    allocator_node_t* node = &allocator->nodes[node_index];
    u32 node_total_size = node->data_size;
    node->data_size = size;
    node->used = true;

    allocator->bin_indices[bin_index] = node->bin_list_next;
    if (node->bin_list_next != ALLOCATION_NO_SPACE) allocator->nodes[node->bin_list_next].bin_list_prev = ALLOCATION_NO_SPACE;
    allocator->free_storage -= node_total_size;

    if (allocator->bin_indices[bin_index] != ALLOCATION_NO_SPACE) {
        allocator->used_bins[top_bin_index] &= ~(1 << leaf_bin_index);

        if (allocator->used_bins[top_bin_index] == 0) {
            allocator->used_bins_top &= ~(1 << top_bin_index);
        }
    }

    u32 reminder_size = node_total_size - size;
    if (reminder_size > 0) {
        u32 new_node_index = allocator_insert_node_into_bin(allocator, reminder_size, node->data_offset + size);

        if (node->neighbor_next != ALLOCATION_NO_SPACE) allocator->nodes[node->neighbor_next].neighbor_prev = new_node_index;
        allocator->nodes[new_node_index].neighbor_prev = node_index;
        allocator->nodes[new_node_index].neighbor_next = node->neighbor_next;
        node->neighbor_next = new_node_index;
    }

    return (allocation_t) {
        .offset = node->data_offset,
        .metadata = node_index
    };
}

void allocator_delete(allocator_t *allocator, allocation_t *allocation)
{
    if (allocation->metadata == ALLOCATION_NO_SPACE) return;
    if (!allocator->nodes) return;

    u32 node_index = allocation->metadata;
    allocator_node_t *node = &allocator->nodes[node_index];

    if (node->used == false) return;

    u32 offset = node->data_offset;
    u32 size = node->data_size;

    if ((node->neighbor_prev != ALLOCATION_NO_SPACE) && (allocator->nodes[node->neighbor_prev].used == false)) {
        allocator_node_t* prev_node = &allocator->nodes[node->neighbor_prev];
        offset += prev_node->data_offset;
        size += prev_node->data_size;

        allocator_remove_node_from_bin(allocator, node->neighbor_prev);

        if (prev_node->neighbor_next != node_index) return;
        node->neighbor_prev = prev_node->neighbor_prev;
    }

    if ((node->neighbor_next != ALLOCATION_NO_SPACE) && (allocator->nodes[node->neighbor_next].used == false)) {
        allocator_node_t* next_node = &allocator->nodes[node->neighbor_next];
        size += next_node->data_size;

        allocator_remove_node_from_bin(allocator, node->neighbor_next);

        if (next_node->neighbor_prev != node_index) return;
        node->neighbor_next = next_node->neighbor_next;
    }

    u32 neighbor_next = node->neighbor_next;
    u32 neighbor_prev = node->neighbor_prev;

    allocator->free_nodes[++allocator->free_offset] = node_index;

    u32 combined_node_index = allocator_insert_node_into_bin(allocator, size, offset);

    if (neighbor_next != ALLOCATION_NO_SPACE) {
        allocator->nodes[combined_node_index].neighbor_next = neighbor_next;
        allocator->nodes[neighbor_next].neighbor_prev = combined_node_index;
    }
    if (neighbor_prev != ALLOCATION_NO_SPACE) {
        allocator->nodes[combined_node_index].neighbor_prev = neighbor_prev;
        allocator->nodes[neighbor_prev].neighbor_next = combined_node_index;
    }
}
#else

void allocator_init(allocator_t *allocator, u32 size)
{
    allocator->offset = 0;
}

void allocator_free(allocator_t *allocator)
{
    
}

allocation_t allocator_new(allocator_t *allocator, u32 size)
{
    allocation_t alloc;
    alloc.offset = allocator->offset;
    alloc.size = size;

    allocator->offset += size;
    return alloc;
}

void allocator_delete(allocator_t *allocator, allocation_t *allocation)
{
    allocation->size = 0;
    allocation->offset = 0;
    allocation->metadata = 0;
}

#endif
