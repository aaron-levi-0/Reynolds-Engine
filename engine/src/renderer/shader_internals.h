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

extern ShaderProgramSource parseFile(const char*);
extern unsigned int CreateShader(char*, char*);
extern int GetUniformLocation(unsigned int , const char* );

extern void bind_shader(unsigned int );
extern void unbind_shader();

#endif