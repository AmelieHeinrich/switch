//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 15:44:55
//

#ifndef GFX_PIPELINE_H_
#define GFX_PIPELINE_H_

#include "gpu.h"

#define MAX_VTX_ATTRIB_STATES 16

typedef struct gfx_pipeline_t {
    gpu_t *gpu;

    // @note(ame): vertex input state
    DkVtxAttribState states[MAX_VTX_ATTRIB_STATES];
    i32 state_count;

    // @note(ame): rasterizier state
    DkFace cull_mode;

    // @note(ame): shaders
    shader_t vert;
    shader_t frag;
} gfx_pipeline_t;

void gfx_pipeline_init(gfx_pipeline_t *pipeline, gpu_t *gpu, const char *v, const char *f);
void gfx_pipeline_free(gfx_pipeline_t *pipeline);

#endif
