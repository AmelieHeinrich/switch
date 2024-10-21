//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 13:48:42
//

#include "cmd_mem_ring.h"

void cmd_mem_ring_init(cmd_mem_ring_t *ring, DkDevice device, heap_t *cmd_heap, i32 ring_size)
{
    ring->ring_size = ring_size;
    ring->curr_slice = 0;
    ring->cmd_heap = cmd_heap;

    u32 slice_size = CMD_BUF_DEFAULT_SIZE;
    slice_size = ALIGN(slice_size, DK_CMDMEM_ALIGNMENT);

    ring->ring_alloc = heap_alloc(cmd_heap, slice_size * ring_size);
}

void cmd_mem_ring_free(cmd_mem_ring_t *ring)
{
    heap_dealloc(ring->cmd_heap, &ring->ring_alloc);
}

void cmd_mem_ring_begin(cmd_mem_ring_t *ring, DkCmdBuf buf)
{
    dkCmdBufClear(buf);

    u32 slice_size = ring->ring_alloc.size / ring->ring_size;
    dkFenceWait(&ring->fences[ring->curr_slice], 5e+9);

    dkCmdBufAddMemory(buf, ring->cmd_heap->block, ring->ring_alloc.offset + (ring->curr_slice * slice_size), slice_size);
}

DkCmdList cmd_mem_ring_end(cmd_mem_ring_t *ring, DkCmdBuf buf)
{
    dkCmdBufSignalFence(buf, &ring->fences[ring->curr_slice], false);

    ring->curr_slice = (ring->curr_slice + 1) % ring->ring_size;
    return dkCmdBufFinishList(buf);
}
