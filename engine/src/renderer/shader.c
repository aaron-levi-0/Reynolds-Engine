#include "renderer/shader_internals.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct cache* cache = NULL;
unsigned int program;

//must implement CreateShader function to free 'source' memory
ShaderProgramSource parseFile(const char* filepath)
{
	ShaderProgramSource source = {0};

	if(!filepath)
	{
		REYNOLDS_WARN("@shader: Shader path not set.");
		return source;
	}

	FILE *fp = fopen(filepath, "r");
	if(!fp)
		REYNOLDS_ERROR("@shader: Could not open file at path: %s", filepath);
	
	enum ShaderType{ NONE = -1, VERTEX, FRAGMENT };
	
	char line[BUFSIZ];
	const char* vertex_header = "#shader vertex";
	const char* fragment_header = "#shader fragment";
	
	enum ShaderType type = NONE;
	char ShaderStream[2][BUFSIZ];
	
	*ShaderStream[0] = '\0';
	*ShaderStream[1] = '\0';
	
	while(fgets(line,sizeof(line), fp))
	{
		if(!strncmp(line, vertex_header, strlen(vertex_header)))
			type = VERTEX;
		
		else if (!strncmp(line, fragment_header, strlen(fragment_header)))
			type = FRAGMENT;
		
		else strcat(ShaderStream[type], line);
		
		//if type = -1 error otherwise it will write in 2. or flag and ignore code up until a header. Also may need to check if cattonated line exceeds BUFSIZ?
	}
	fclose(fp);

	source.VertexSource = malloc(strlen(ShaderStream[0]) + 1);
	strcpy(source.VertexSource, ShaderStream[0]);
	source.FragmentSource = malloc(strlen(ShaderStream[1]) + 1);
	strcpy(source.FragmentSource, ShaderStream[1]);
	
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
		REYNOLDS_ERROR("OpenGL Error: %s", message);
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

int GetUniformLocation(unsigned int programID, const char* name)
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
	int location = glGetUniformLocation(program, name);
	glUniform1iv(location, count, array);
}

void setMat4(const char* name, mat4 array)
{
	int location = glGetUniformLocation(program, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, (float* )array);
}