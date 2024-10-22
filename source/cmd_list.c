//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 15:35:49
//

#include "cmd_list.h"

void cmd_list_viewport_scissor(DkCmdBuf buf, f32 x, f32 y)
{
    DkViewport viewport = { 0.0f, 0.0f, (float)x, (float)y, 0.0f, 1.0f };
    DkScissor scissor = { 0, 0, x, y };

    dkCmdBufSetViewports(buf, 0, &viewport, 1);
    dkCmdBufSetScissors(buf, 0, &scissor, 1);
}

void cmd_list_clear_color(DkCmdBuf buf, HMM_Vec3 col, u32 idx)
{
    dkCmdBufClearColorFloat(buf, idx, DkColorMask_RGBA, col.X, col.Y, col.Z, 1.0f);
}

void cmd_list_bind_vtx_buffer(DkCmdBuf buf, buffer_t *buffer)
{
    DkVtxBufferState state = {
        .stride = buffer->stride,
        .divisor = 0

    };
    dkCmdBufBindVtxBufferState(buf, &state, 1);
    dkCmdBufBindVtxBuffer(buf, 0, buffer->mem.gpu_addr, buffer->mem.size);
}

void cmd_list_bind_idx_buffer(DkCmdBuf buf, buffer_t *buffer)
{
    dkCmdBufBindIdxBuffer(buf, DkIdxFormat_Uint32, buffer->mem.gpu_addr);
}

void cmd_list_bind_uni_buffer(DkCmdBuf buf, buffer_t *buffer, u32 idx, DkStage stage)
{
    dkCmdBufBindUniformBuffer(buf, stage, idx, buffer->mem.gpu_addr, buffer->mem.size);
}

void cmd_list_draw(DkCmdBuf buf, DkPrimitive prim, u32 vtx)
{
    dkCmdBufDraw(buf, prim, vtx, 1, 0, 0);
}

void cmd_list_draw_indexed(DkCmdBuf buf, DkPrimitive prim, u32 idx)
{
    dkCmdBufDrawIndexed(buf, prim, idx, 1, 0, 0, 0);
}

void cmd_list_bind_gfx_pipeline(DkCmdBuf buf, gfx_pipeline_t *pipeline)
{
    DkShader const* shaders[] = { &pipeline->vert.shader, &pipeline->frag.shader };
    DkRasterizerState rasterizerState;
    DkColorState colorState;
    DkColorWriteState colorWriteState;

    // Initialize state structs with the deko3d defaults
    dkRasterizerStateDefaults(&rasterizerState);
    dkColorStateDefaults(&colorState);
    dkColorWriteStateDefaults(&colorWriteState);

    rasterizerState.cullMode = DkFace_None;

    dkCmdBufBindShaders(buf, DkStageFlag_GraphicsMask, shaders, 2);
    dkCmdBufBindRasterizerState(buf, &rasterizerState);
    dkCmdBufBindColorState(buf, &colorState);
    dkCmdBufBindColorWriteState(buf, &colorWriteState);
    dkCmdBufBindVtxAttribState(buf, pipeline->states, pipeline->state_count);
}
