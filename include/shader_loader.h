//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 14:44:07
//

#ifndef SHADER_LOADER_H_
#define SHADER_LOADER_H_

#define SHADER_MEM_SIZE MEGABYTES(8)

#include "heap.h"

// @todo(ame): Switch to arena but I just don't trust myself rn LOL
typedef struct shader_t {
    DkShader shader;
    heap_alloc_t alloc;
} shader_t;

typedef struct shader_loader_t {
    heap_t shader_heap;
} shader_loader_t;

void shader_loader_init(shader_loader_t *loader, DkDevice device);
shader_t shader_loader_read(shader_loader_t *loader, const char *path);
void shader_loader_free_shader(shader_loader_t *loader, shader_t *shader);
void shader_loader_free(shader_loader_t *loader);

#endif
