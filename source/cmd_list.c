//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 15:35:49
//

#include "cmd_list.h"

cmd_list_single_use_t cmd_list_begin_single_use(DkDevice device, heap_t *heap)
{
    DkCmdBufMaker maker;
    dkCmdBufMakerDefaults(&maker, device);
    DkCmdBuf buf = dkCmdBufCreate(&maker);

    heap_alloc_t alloc = heap_alloc(heap, KILOBYTES(8));
    dkCmdBufAddMemory(buf, heap->block, alloc.offset, alloc.size);

    return (cmd_list_single_use_t){
        .alloc = alloc,
        .buf = buf,
        .parent_heap = heap
    };
}

void cmd_list_end_single_use(cmd_list_single_use_t *buf, DkQueue queue)
{
    dkQueueSubmitCommands(queue, dkCmdBufFinishList(buf->buf));
}

void cmd_list_free_single_use(cmd_list_single_use_t *buf)
{
    heap_dealloc(buf->parent_heap, &buf->alloc);
    dkCmdBufDestroy(buf->buf);
}

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

void cmd_list_copy_buffer_to_texture(DkCmdBuf buf, buffer_t *src, texture_t *dst)
{
    DkCopyBuf copy;
    copy.addr = src->mem.gpu_addr;
    copy.imageHeight = 0;
    copy.rowLength = 0;

    DkImageRect dest = {
        .x = 0,
        .y = 0,
        .z = 0,
        .width = dst->width,
        .height = dst->height,
        .depth = 1
    };

    dkCmdBufCopyBufferToImage(buf, &copy, &dst->image_view, &dest, 0);
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

void cmd_list_dset_write_texture(DkCmdBuf buf, descriptor_set_t *set, texture_t *texture, u32 idx)
{
    u32 aligned_size = ALIGN(sizeof(DkImageDescriptor), DK_IMAGE_DESCRIPTOR_ALIGNMENT);
    dkCmdBufPushData(buf, set->mem.gpu_addr + (idx * aligned_size), &texture->descriptor, aligned_size);
}

void cmd_list_dset_write_sampler(DkCmdBuf buf, descriptor_set_t *set, sampler_t *sampler, u32 idx)
{
    u32 aligned_size = ALIGN(sizeof(DkImageDescriptor), DK_IMAGE_DESCRIPTOR_ALIGNMENT);
    dkCmdBufPushData(buf, set->mem.gpu_addr + (idx * aligned_size), &sampler->descriptor, aligned_size);
}

void cmd_list_bind_image_dset(DkCmdBuf buf, descriptor_set_t *set)
{
    dkCmdBufBindImageDescriptorSet(buf, set->mem.gpu_addr, set->descriptor_count);
}

void cmd_list_bind_sampler_dset(DkCmdBuf buf, descriptor_set_t *set)
{
    dkCmdBufBindSamplerDescriptorSet(buf, set->mem.gpu_addr, set->descriptor_count);
}

void cmd_list_bind_texture(DkCmdBuf buf, DkStage stage, u32 txt_idx, u32 smp_idx)
{
    dkCmdBufBindTexture(buf, stage, 0, dkMakeTextureHandle(txt_idx, smp_idx));
}
