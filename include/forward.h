//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-23 15:17:38
//

#ifndef FORWARD_H_
#define FORWARD_H_

#include "buffer.h"
#include "texture.h"
#include "descriptor_set.h"
#include "gfx_pipeline.h"
#include "sampler.h"

typedef struct forward_t {
    gpu_t *gpu;

    texture_t depth;
    buffer_t camera_buffer[DEFAULT_GPU_FB_COUNT];
    gfx_pipeline_t pipeline;
    descriptor_set_t texture_set;
    descriptor_set_t sampler_set;
    sampler_t sampler;
} forward_t;

void forward_init(forward_t *forward, gpu_t *gpu);
void forward_render(forward_t *forward, frame_t *frame);
void forward_free(forward_t *forward);

#endif
