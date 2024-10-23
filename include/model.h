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
#define MAX_MATERIALS 512

typedef struct vertex_t {
    HMM_Vec3 position;
    HMM_Vec2 uv;
    HMM_Vec3 normals;
} vertex_t;

typedef struct material_t {
    b8 has_albedo;
    b8 has_bump;

    texture_t albedo;
    texture_t bump;
} material_t;

typedef struct submesh_t {
    buffer_t vertex_buffer;
    buffer_t index_buffer;
    buffer_t user_buffer[DEFAULT_GPU_FB_COUNT];

    u32 vertex_count;
    u32 index_count;
    u32 mat_index;

    HMM_Mat4 transform;
} submesh_t;

typedef struct model_t {
    submesh_t *submeshes;
    u32 submesh_count;

    material_t *materials;
    u32 material_count;

    u32 vertex_count;
    u32 index_count;
    
    char* directory;
    gpu_t *gpu;
} model_t;

void model_load(model_t *model, gpu_t *gpu, const char *path);
void model_free(model_t *model);

#endif
