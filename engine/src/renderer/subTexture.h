#ifndef SUBTEXTURE_H
#define SUBTEXTURE_H

#include <stdint.h>
#include <stdbool.h>

#include "core/glx.h"
#include "utils/vector.h"

// A named region of a texture atlas/spritesheet
typedef struct {
    uint32_t textureID;
    uint16_t cell_location[2];  // by cell count, not in pixels
    vec4 uv;                    // {u0, v0, u1, v1} in normalized 0..1 coords
} SubTexture;

extern void subtex_init(Vector* reg, size_t initial_capacity);
extern void subtex_shutdown(Vector* reg);

// Register using pixel coordinates within the spritesheet.
// pixel_pos: top-left in pixels (or bottom-left depending on your convention)
// pixel_size: width/height in pixels
extern bool subtex_register_px(Vector* reg,
                        uint32_t textureID,
                        vec2 pixel_pos,
                        vec2 pixel_size);

extern bool subtex_register_cell(Vector* reg,
                        uint32_t textureID,
                        uint16_t cell_pos[2],
                        vec2 cell_size,
                        vec2 sample_size);

// Lookup by name. Returns NULL if missing.
extern float* get_uv(const Vector* entries, uint32_t textureID, uint16_t cell_location[2]);

// Convenience: returns pointer to uv vec4, or NULL if missing.
// static inline const vec4* subtex_uv(const SubTextureRegistry* reg, const char* name)
// {
//     const SubTexture* st = subtex_get(reg, name);
//     return st ? &st->uv : NULL;
// }

#endif // SUBTEXTURE_H
