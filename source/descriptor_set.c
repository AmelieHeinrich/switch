//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-23 00:06:34
//

#include "descriptor_set.h"

descriptor_set_t descriptor_set_init(heap_t *heap, u32 descriptor_count)
{
    u32 aligned_size = ALIGN(sizeof(DkImageDescriptor), DK_IMAGE_DESCRIPTOR_ALIGNMENT);

    return (descriptor_set_t){
        .parent_heap = heap,
        .mem = heap_alloc(heap, aligned_size * descriptor_count),
        .descriptor_count = descriptor_count,
    };
}

void descriptor_set_free(descriptor_set_t *set)
{
    heap_dealloc(set->parent_heap, &set->mem);
}
