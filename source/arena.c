//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 18:54:07
//

#include "arena.h"

#include <stdlib.h>

void arena_init_default(arena_t *arena, u64 size)
{
    arena->size = size;
    arena->offset = 0;
    arena->cpu = true;
    
    arena->cpu_block = malloc(size);
}

void arena_init_gpu(arena_t *arena, u64 size, u32 flags, DkDevice device)
{
    arena->size = size;
    arena->offset = 0;
    arena->cpu = false;

    DkMemBlockMaker mem_maker;
    dkMemBlockMakerDefaults(&mem_maker, device, size);
    mem_maker.flags = flags;
    arena->gpu_block = dkMemBlockCreate(&mem_maker);
}

void arena_free(arena_t *arena)
{
    if (arena->cpu) {
        free(arena->cpu_block);
    } else {
        dkMemBlockDestroy(arena->gpu_block);
    }
}
