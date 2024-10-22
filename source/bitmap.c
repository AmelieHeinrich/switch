//
// $Notice: Xander Studios @ 2024
// $Author: Amélie Heinrich
// $Create Time: 2024-10-22 22:17:02
//

#include <stb_image.h>

#include "bitmap.h"
#include "util.h"

bitmap_t bitmap_load(const char *path)
{
    i32 w, h, c;
    stbi_set_flip_vertically_on_load(true);
    u8* p = stbi_load(path, &w, &h, &c, STBI_rgb_alpha);
    if (!p || w <= 0 || h <= 0) {
        throw_error("Failed to load texture", "Failed to load texture", -1);
    }

    return (bitmap_t){
        .width = w,
        .height = h,
        .pixels = p
    };
}

void bitmap_free(bitmap_t *bitmap)
{
    stbi_image_free(bitmap->pixels);
}
