#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>
#include <cglm/cglm.h>
#include "defines.h"

REN_API void setIntArray(const char* , int* , uint32_t );
REN_API void setMat4(const char* , mat4 );

#endif