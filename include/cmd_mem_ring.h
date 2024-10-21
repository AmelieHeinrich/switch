//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 13:28:22
//

#ifndef CMD_MEM_RING_H_
#define CMD_MEM_RING_H_

#define CMD_BUF_DEFAULT_SIZE 0x10000
#define CMD_MEM_MAX_RING_SIZE 3

#include <deko3d.h>

#include "arena.h"

typedef struct cmd_mem_ring_t {
    arena_t arena;

    DkFence fences[CMD_MEM_MAX_RING_SIZE];
    i32 ring_size;
    u32 curr_slice;
} cmd_mem_ring_t;

void cmd_mem_ring_init(cmd_mem_ring_t *ring, DkDevice device, i32 ring_size);
void cmd_mem_ring_free(cmd_mem_ring_t *ring);
void cmd_mem_ring_begin(cmd_mem_ring_t *ring, DkCmdBuf buf);
DkCmdList cmd_mem_ring_end(cmd_mem_ring_t *ring, DkCmdBuf buf);

#endif
