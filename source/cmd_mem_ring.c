//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 13:48:42
//

#include "cmd_mem_ring.h"

void cmd_mem_ring_init(cmd_mem_ring_t *ring, DkDevice device, i32 ring_size)
{
    ring->ring_size = ring_size;
    ring->curr_slice = 0;

    u32 slice_size = CMD_BUF_DEFAULT_SIZE;
    slice_size = ALIGN(slice_size, DK_CMDMEM_ALIGNMENT);

    arena_init_gpu(&ring->arena, slice_size * ring_size, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached, device);
}

void cmd_mem_ring_free(cmd_mem_ring_t *ring)
{
    arena_free(&ring->arena);
}

void cmd_mem_ring_begin(cmd_mem_ring_t *ring, DkCmdBuf buf)
{
    dkCmdBufClear(buf);

    u32 slice_size = ring->arena.size / ring->ring_size;
    dkFenceWait(&ring->fences[ring->curr_slice], 5e+9);

    dkCmdBufAddMemory(buf, ring->arena.gpu_block, ring->curr_slice * slice_size, slice_size);
}

DkCmdList cmd_mem_ring_end(cmd_mem_ring_t *ring, DkCmdBuf buf)
{
    dkCmdBufSignalFence(buf, &ring->fences[ring->curr_slice], false);

    ring->curr_slice = (ring->curr_slice + 1) % ring->ring_size;
    return dkCmdBufFinishList(buf);
}
