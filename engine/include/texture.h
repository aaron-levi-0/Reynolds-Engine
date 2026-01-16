#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdint.h>
#include "defines.h"

#define	MAX_TEXTURE_SLOTS	32

REN_API uint32_t LoadTexture(const char* filepath);

#endif //TEXTURE_H