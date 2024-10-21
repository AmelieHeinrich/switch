//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-21 14:53:53
//

#include <stdio.h>

#include "shader_loader.h"

void shader_loader_init(shader_loader_t *loader, DkDevice device)
{
    DkMemBlockMaker maker;
    dkMemBlockMakerDefaults(&maker, device, SHADER_MEM_SIZE);
    maker.flags = DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached | DkMemBlockFlags_Code;
    loader->code_block = dkMemBlockCreate(&maker);
}

DkShader shader_loader_read(shader_loader_t *loader, const char *path)
{
    // Open the file, and retrieve its size
    FILE* f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    u32 size = ftell(f);
    rewind(f);

    // Look for a spot in the code memory block for loading this shader. Note that
    // we are just using a simple incremental offset; this isn't a general purpose
    // allocation algorithm.
    u32 code_offset = loader->offset;
    loader->offset += ALIGN(size, DK_SHADER_CODE_ALIGNMENT);

    // Read the file into memory, and close the file
    fread((u8*)dkMemBlockGetCpuAddr(loader->code_block) + code_offset, size, 1, f);
    fclose(f);

    // Initialize the user provided shader object with the code we've just loaded
    DkShader shader;
    DkShaderMaker shaderMaker;
    dkShaderMakerDefaults(&shaderMaker, loader->code_block, code_offset);
    dkShaderInitialize(&shader, &shaderMaker);

    return shader;
}

void shader_loader_free(shader_loader_t *loader)
{
    dkMemBlockDestroy(loader->code_block);
}
