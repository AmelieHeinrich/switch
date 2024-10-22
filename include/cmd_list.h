//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 15:32:20
//

#ifndef CMD_LIST_H_
#define CMD_LIST_H_

#include <HandmadeMath.h>

#include "core.h"
#include "gpu.h"
#include "gfx_pipeline.h"
#include "buffer.h"
#include "texture.h"
#include "sampler.h"
#include "descriptor_set.h"

typedef struct cmd_list_single_use_t {
    DkCmdBuf buf;
    heap_alloc_t alloc;
    heap_t *parent_heap;
} cmd_list_single_use_t;

// upload
cmd_list_single_use_t cmd_list_begin_single_use(DkDevice device, heap_t *heap);
void cmd_list_end_single_use(cmd_list_single_use_t *buf, DkQueue queue);
void cmd_list_free_single_use(cmd_list_single_use_t *buf);

// state
void cmd_list_viewport_scissor(DkCmdBuf buf, f32 w, f32 h);

// bind
void cmd_list_bind_vtx_buffer(DkCmdBuf buf, buffer_t *buffer);
void cmd_list_bind_idx_buffer(DkCmdBuf buf, buffer_t *buffer);
void cmd_list_bind_uni_buffer(DkCmdBuf buf, buffer_t *buffer, u32 idx, DkStage stage);
void cmd_list_bind_image_dset(DkCmdBuf buf, descriptor_set_t *set);
void cmd_list_bind_sampler_dset(DkCmdBuf buf, descriptor_set_t *set);
void cmd_list_bind_gfx_pipeline(DkCmdBuf buf, gfx_pipeline_t *pipeline);
void cmd_list_bind_texture(DkCmdBuf buf, DkStage stage, u32 txt_idx, u32 smp_idx);

// dset
void cmd_list_dset_write_texture(DkCmdBuf buf, descriptor_set_t *set, texture_t *texture, u32 idx);
void cmd_list_dset_write_sampler(DkCmdBuf buf, descriptor_set_t *set, sampler_t *sampler, u32 idx);

// clear
void cmd_list_clear_color(DkCmdBuf buf, HMM_Vec3 col, u32 idx);

// transfer
void cmd_list_copy_buffer_to_texture(DkCmdBuf buf, buffer_t *src, texture_t *dst);

// draw/dispatch
void cmd_list_draw(DkCmdBuf buf, DkPrimitive prim, u32 vtx);
void cmd_list_draw_indexed(DkCmdBuf buf, DkPrimitive prim, u32 idx);

#endif
