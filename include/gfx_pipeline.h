//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 15:44:55
//

#ifndef GFX_PIPELINE_H_
#define GFX_PIPELINE_H_

#include "gpu.h"

typedef struct gfx_pipeline_t {
    // @todo(ame): vertex state
    DkFace cull_mode;

    // @note(ame): shaders
    DkShader vert;
    DkShader frag;
} gfx_pipeline_t;

void gfx_pipeline_init(gfx_pipeline_t *pipeline, gpu_t *gpu, const char *v, const char *f);
void gfx_pipeline_free(gfx_pipeline_t *pipeline);

#endif
