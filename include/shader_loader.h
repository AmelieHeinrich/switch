//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 14:44:07
//

#ifndef SHADER_LOADER_H_
#define SHADER_LOADER_H_

#define SHADER_MEM_SIZE MEGABYTES(8)

#include "arena.h"

// @todo(ame): Switch to arena but I just don't trust myself rn LOL
typedef struct shader_loader_t {
    DkMemBlock code_block;
    u32 offset;
} shader_loader_t;

void shader_loader_init(shader_loader_t *loader, DkDevice device);
DkShader shader_loader_read(shader_loader_t *loader, const char *path);
void shader_loader_free(shader_loader_t *loader);

#endif
