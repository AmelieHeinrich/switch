//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-23 14:18:36
//

#ifndef MODEL_H_
#define MODEL_H_

#include "core.h"
#include "gpu.h"
#include "buffer.h"
#include "texture.h"

#define MAX_SUBMESHES 512

typedef struct vertex_t {
    HMM_Vec3 position;
    HMM_Vec2 uv;
    HMM_Vec3 normals;
} vertex_t;

// @todo(ame): materials

typedef struct submesh_t {
    buffer_t vertex_buffer;
    buffer_t index_buffer;

    u32 vertex_count;
    u32 index_count;

    HMM_Mat4 transform;
} submesh_t;

typedef struct model_t {
    submesh_t *submeshes;
    u32 submesh_count;

    u32 vertex_count;
    u32 index_count;
    
    char* directory;
    gpu_t *gpu;
} model_t;

void model_load(model_t *model, gpu_t *gpu, const char *path);
void model_free(model_t *model);

#endif
