#include "renderer/shader_internals.h"

#include <GL/glew.h>

struct cache* cache = NULL;
unsigned int program;

//must implement CreateShader function to free 'source' memory
ShaderProgramSource parseFile(const char* filepath)
{
	ShaderProgramSource source = {0};

	enum ShaderType{ NONE = -1, VERTEX, FRAGMENT };
	enum ShaderType type = NONE;
	
	char line[BUFSIZ];
	const char* vertex_header = "#shader vertex";
	const char* fragment_header = "#shader fragment";

	if(!filepath)
	{
		REYNOLDS_WARN("@shader: Shader path not set.");
		return source;
	}

	FILE *fp = fopen(filepath, "r");
	if(!fp)
	{
		REYNOLDS_ERROR("@shader: Could not open file at path: %s", filepath);
		return source;
	}
	
	char* ShaderStream[2]	= {malloc(BUFSIZ), malloc(BUFSIZ)};
	size_t capacity[2]		= { BUFSIZ, BUFSIZ };

	if(!ShaderStream[0] || !ShaderStream[1])
	{
		REYNOLDS_ERROR("@shader: Could not allocate memory for shader source.");

		free(ShaderStream[0]);
		free(ShaderStream[1]);
		fclose(fp);
		return source;
	}

	*ShaderStream[0] = '\0';
	*ShaderStream[1] = '\0';
	
	while(fgets(line,sizeof(line), fp))
	{
		if(!strncmp(line, vertex_header, strlen(vertex_header)))
			type = VERTEX;
		
		else if (!strncmp(line, fragment_header, strlen(fragment_header)))
			type = FRAGMENT;
		
		else if (type != NONE)					//ignores any text before the first header and any text after the last header
		{
			size_t needed = strlen(ShaderStream[type]) + strlen(line) + 1;

			if (needed > capacity[type]) 
			{
				size_t new_cap = capacity[type];
				while (new_cap < needed) new_cap *= 2;
				
				char* grown = realloc(ShaderStream[type], new_cap);
				if (!grown) 
				{ 
					REYNOLDS_ERROR("@shader: OOM growing shader source.");
					free(ShaderStream[0]); free(ShaderStream[1]); fclose(fp); 
					return source; 
				}

				ShaderStream[type] = grown;
				capacity[type]     = new_cap;
			}
			strcat(ShaderStream[type], line);	
		}
	}

	fclose(fp);

	source.VertexSource 	= ShaderStream[0];
	source.FragmentSource 	= ShaderStream[1];
	
	return source;
}

unsigned int CompileShader(const char* source, unsigned int type)
{
	unsigned int id = glCreateShader(type);
	glShaderSource(id, 1, &source, NULL);
	glCompileShader(id);
	
	/* Error handling*/
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length*sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		
		const char* shader = type == GL_VERTEX_SHADER ? "vertex": "fragment";
		REYNOLDS_ERROR("@shader: Failed to compile %s", shader);
		glDeleteShader(id);
		return 0;
	}
	return id;
}

unsigned int CreateShader(char* vertexSource, char* fragmentSource)
{
	program = glCreateProgram();
	unsigned int vs = CompileShader(vertexSource, GL_VERTEX_SHADER);
	unsigned int fs = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
	
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	
	glDeleteShader(vs);
	glDeleteShader(fs);
	
	free(vertexSource);
	free(fragmentSource);
	
	return program;
}

void bind_shader(unsigned int programID)
{
	glUseProgram(programID);
}

void unbind_shader()
{
	glUseProgram(0);
}

//finds uniform in shader and caches value for quick access next call

static int GetUniformLocation(unsigned int programID, const char* name)
{
	int location, cached_location;

	if(!cache) cache = create_cache(CACHE_LIMIT);

	cached_location = cache_get(cache, name);
	if (cached_location != -1)
		return cached_location;

	location = glGetUniformLocation(programID, name);
	if (location == -1)
		REYNOLDS_ERROR("@shader: uniform %s does not exist!", name);
	
	cache_set(cache, name, location);
	return location;
}

void setIntArray(const char* name, int* array, uint32_t count)
{
	int location = GetUniformLocation(program, name);
	glUniform1iv(location, count, array);
}

void setMat4(const char* name, mat4 array)
{
	int location = GetUniformLocation(program, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, (float* )array);
}