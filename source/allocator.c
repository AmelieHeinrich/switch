//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 17:06:50
//

#include <stdlib.h>

#include "allocator.h"

#define MANTISSA_BITS 3
#define MANTISSA_VALUE 1 << MANTISSA_BITS
#define MANTISSA_MASK MANTISSA_VALUE - 1

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

void allocator_init(allocator_t *allocator, u32 size)
{
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
}

allocator_node_t allocator_new(u32 size)
{

}

void allocator_delete(allocator_node_t *node)
{

}
