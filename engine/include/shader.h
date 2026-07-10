#ifndef SHADER_H
#define SHADER_H

#include <stdint.h>
#include <cglm/cglm.h>
#include "defines.h"

struct Shader;

REN_API struct Shader* LoadShader(const char* filepath);
REN_API void FreeShader(struct Shader* shader);
REN_API void SetIntArray(struct Shader* shader, const char* name, int* array, uint32_t count);
REN_API void setMat4(struct Shader* shader, const char* name, mat4 array);

#endif