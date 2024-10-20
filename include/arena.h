//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-20 18:38:50
//

#ifndef ARENA_H_
#define ARENA_H_

#include <deko3d.h>
#include <switch.h>

#include "core.h"

#define KILOBYTES(b) b * 1024
#define MEGABYTES(b) KILOBYTES(b) * 1024
#define GIGABYTES(b) MEGABYTES(b) * 1024

typedef enum arena_result {
    /// @note(ame): all gud
    ArenaResult_Success = BIT(0),
    /// @note(ame): not gud
    ArenaResult_OutOfMem = BIT(1),
    /// @note(ame): align it better skibidi
    ArenaResult_WrongAlignment = BIT(2)
} arena_result;

typedef struct arena_t {
    u32 flags;
    u64 size;
    u64 offset;
    b8 cpu;

    // @note(ame): Only for ArenaFlags_Default
    void *cpu_block;
    // @note(ame): Everything else
    DkMemBlock gpu_block;
} arena_t;

void arena_init_default(arena_t *arena, u64 size);
void arena_init_gpu(arena_t *arena, u64 size, u32 flags, DkDevice device);
void arena_free(arena_t *arena);

#endif
