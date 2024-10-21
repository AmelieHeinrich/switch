//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 14:53:53
//

#include <stdio.h>

#include "shader_loader.h"

void shader_loader_init(shader_loader_t *loader, DkDevice device)
{
    heap_init(&loader->shader_heap, SHADER_MEM_SIZE, DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code, device);
}

shader_t shader_loader_read(shader_loader_t *loader, const char *path)
{
    // Open the file, and retrieve its size
    FILE* f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    u32 size = ftell(f);
    rewind(f);

    // Look for a spot in the code memory block for loading this shader. Note that
    // we are just using a simple incremental offset; this isn't a general purpose
    // allocation algorithm.
    heap_alloc_t alloc = heap_alloc(&loader->shader_heap, ALIGN(size, DK_SHADER_CODE_ALIGNMENT));

    // Read the file into memory, and close the file
    fread((u8*)alloc.cpu_addr, size, 1, f);
    fclose(f);

    // Initialize the user provided shader object with the code we've just loaded
    DkShader shader;
    DkShaderMaker shaderMaker;
    dkShaderMakerDefaults(&shaderMaker, loader->shader_heap.block, alloc.offset);
    dkShaderInitialize(&shader, &shaderMaker);

    return (shader_t) {
        .alloc = alloc,
        .shader = shader
    };
}

void shader_loader_free_shader(shader_loader_t *loader, shader_t *shader)
{
    heap_dealloc(&loader->shader_heap, &shader->alloc);
}

void shader_loader_free(shader_loader_t *loader)
{
    heap_free(&loader->shader_heap);
}
