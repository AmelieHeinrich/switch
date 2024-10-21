//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 19:16:03
//

#include "heap.h"
#include "util.h"

void heap_init(heap_t *heap, u64 size, u32 flags, DkDevice device)
{
    allocator_init(&heap->allocator, size);

    DkMemBlockMaker maker;
    dkMemBlockMakerDefaults(&maker, device, size);
    maker.flags = flags;
    heap->block = dkMemBlockCreate(&maker);
}

void heap_free(heap_t *heap)
{
    dkMemBlockDestroy(heap->block);
    allocator_free(&heap->allocator);
}

heap_alloc_t heap_alloc(heap_t *heap, u32 size)
{
    allocation_t alloc = allocator_new(&heap->allocator, size);
    if (alloc.offset == ALLOCATION_NO_SPACE) {
        throw_error("Heap Error", "Out of Memory", 1);
    }

    return (heap_alloc_t) {
        .offset = alloc.offset,
        .size = size,
        .internal_allocation = alloc,
        .valid = true,
        .cpu_addr = ((char*)dkMemBlockGetCpuAddr(heap->block) + alloc.offset),
        .gpu_addr = dkMemBlockGetGpuAddr(heap->block) + alloc.offset
    };
}

void heap_dealloc(heap_t *heap, heap_alloc_t *alloc)
{
    allocator_delete(&heap->allocator, &alloc->internal_allocation);
    alloc->valid = false;
}
