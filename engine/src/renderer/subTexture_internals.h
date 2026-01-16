#ifndef SUBTEXTURE_INTERNALS_H
#define SUBTEXTURE_INTERNALS_H

#include "subTexture.h"

// A named region of a texture atlas/spritesheet
typedef struct {
    uint32_t textureID;
    uint16_t cell_location[2];  // by cell count, not in pixels
    vec4 uv;                    // {u0, v0, u1, v1} in normalized 0..1 coords
} SubTexture;

// Convenience: returns pointer to uv vec4, or NULL if missing.
// static inline const vec4* subtex_uv(const SubTextureRegistry* reg, const char* name)
// {
//     const SubTexture* st = subtex_get(reg, name);
//     return st ? &st->uv : NULL;
// }

#endif // SUBTEXTURE_INTERNALS_H
