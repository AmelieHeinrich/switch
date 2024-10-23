//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-22 22:39:35
//

#include "util.h"
#include "texture.h"

void texture_init(texture_t *texture, u32 width, u32 height, DkImageFormat format, heap_t *heap, DkDevice device, u32 flags)
{
    texture->width = width;
    texture->height = height;
    texture->format = format;
    texture->parent_heap = heap;
    texture->flags = flags;

    DkImageLayoutMaker maker;
    dkImageLayoutMakerDefaults(&maker, device);
    maker.format = format;
    maker.flags = flags;
    maker.dimensions[0] = width;
    maker.dimensions[1] = height;
    dkImageLayoutInitialize(&texture->layout, &maker);

    texture->size = dkImageLayoutGetSize(&texture->layout);
    texture->alignment = dkImageLayoutGetAlignment(&texture->layout);
    texture->alloc = heap_alloc(heap, texture->size);

    dkImageInitialize(&texture->image, &texture->layout, heap->block, texture->alloc.offset);
    dkImageViewDefaults(&texture->image_view, &texture->image);
    dkImageDescriptorInitialize(&texture->descriptor, &texture->image_view, false, false);
}

void texture_free(texture_t *texture)
{
    heap_dealloc(texture->parent_heap, &texture->alloc);
}
