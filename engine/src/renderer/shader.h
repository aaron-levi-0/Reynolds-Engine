#ifndef SHADER_H
#define SHADER_H

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

extern void setIntArray(const char* , int* , uint32_t );
extern void setMat4(const char* , mat4 );

#endif