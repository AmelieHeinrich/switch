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

void cmd_list_viewport_scissor(DkCmdBuf buf, f32 w, f32 h);

void cmd_list_bind_vtx_buffer(DkCmdBuf buf, buffer_t *buffer);
void cmd_list_bind_idx_buffer(DkCmdBuf buf, buffer_t *buffer);
void cmd_list_bind_uni_buffer(DkCmdBuf buf, buffer_t *buffer, u32 idx, DkStage stage);
void cmd_list_bind_gfx_pipeline(DkCmdBuf buf, gfx_pipeline_t *pipeline);

void cmd_list_clear_color(DkCmdBuf buf, HMM_Vec3 col, u32 idx);

void cmd_list_draw(DkCmdBuf buf, DkPrimitive prim, u32 vtx);
void cmd_list_draw_indexed(DkCmdBuf buf, DkPrimitive prim, u32 idx);

#endif
