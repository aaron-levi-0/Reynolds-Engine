#ifndef TEXTURE_H
#define TEXTURE_H

#include "core/glx.h"

#define	MAX_TEXTURE_SLOTS	32


extern void init_asset_manager();
extern void create_texture(uint32_t* );
extern void setWhiteTexture(uint32_t );

REN_API unsigned int load_texture(const char*, uint32_t, uint32_t );
extern float* createTexCoords(uint32_t , vec2, vec2 );
extern void getTextureSize(uint32_t textureID, uint32_t* size);

#endif //TEXTURE_H