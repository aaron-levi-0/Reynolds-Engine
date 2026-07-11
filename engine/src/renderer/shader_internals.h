#ifndef SHADER_INTERNALS_H
#define SHADER_INTERNALS_H

#include "shader.h"
#include "utils/hash.h"

#define		CACHE_LIMIT		100

typedef struct
{
	char* VertexSource;
	char* FragmentSource;
}ShaderProgramSource;

struct Shader
{
	uint32_t programID;
	struct cache* uniforms;
};

extern uint32_t CreateShader(char*, char*);

extern void BindShader(struct Shader* shader);
extern void UnbindShader();

#endif