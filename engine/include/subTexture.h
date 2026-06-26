#ifndef SUBTEXTURE_H
#define SUBTEXTURE_H

#include <stdint.h>
#include <stdbool.h>

#include <cglm/cglm.h>
#include "defines.h"
#include "utils/vector.h"

REN_API void subtex_init(Vector* reg, size_t initial_capacity);
REN_API void subtex_shutdown(Vector* reg);

// Register using pixel coordinates within the spritesheet.
// pixel_pos: top-left in pixels (or bottom-left depending on your convention)
// pixel_size: width/height in pixels
REN_API bool subtex_register_px(Vector* reg,
                        uint32_t textureID,
                        vec2 pixel_pos,
                        vec2 pixel_size);

REN_API bool subtex_register_cell(Vector* reg,
                        uint32_t textureID,
                        uint16_t cell_pos[2],
                        vec2 cell_size,
                        vec2 sample_size);

// Lookup by name. Returns NULL if missing.
REN_API float* get_uv(const Vector* entries, uint32_t textureID, uint16_t cell_location[2]);

// Convenience: returns pointer to uv vec4, or NULL if missing.
// static inline const vec4* subtex_uv(const SubTextureRegistry* reg, const char* name)
// {
//     const SubTexture* st = subtex_get(reg, name);
//     return st ? &st->uv : NULL;
// }

#endif // SUBTEXTURE_H
