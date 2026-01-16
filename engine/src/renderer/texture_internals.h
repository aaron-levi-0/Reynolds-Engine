#ifndef TEXTURE_INTERNALS_H
#define TEXTURE_INTERNALS_H

#include "texture.h"

#include <stdint.h>
#include <cglm/cglm.h>
#include "defines.h"

extern void init_asset_manager();
extern void create_texture(uint32_t* );
extern void setWhiteTexture(uint32_t );

extern float* createTexCoords(uint32_t , vec2, vec2 );
extern void getTextureSize(uint32_t textureID, uint32_t* size);

#endif //TEXTURE_INTERNALS_H