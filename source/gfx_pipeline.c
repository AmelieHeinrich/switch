//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 15:48:59
//

#include "gfx_pipeline.h"

void gfx_pipeline_init(gfx_pipeline_t *pipeline, gpu_t *gpu, const char *v, const char *f)
{
    pipeline->cull_mode = DkFace_Back;

    pipeline->vert = shader_loader_read(&gpu->shader_loader, v);
    pipeline->frag = shader_loader_read(&gpu->shader_loader, f);
}

void gfx_pipeline_free(gfx_pipeline_t *pipeline)
{
    // @note(ame): empty
}
