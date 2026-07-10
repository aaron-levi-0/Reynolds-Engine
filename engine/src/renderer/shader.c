#include "renderer/shader_internals.h"

#include <GL/glew.h>

uint32_t currently_bound = 0;

struct cache* cache = NULL;

//must implement CreateShader function to free 'source' memory
static ShaderProgramSource parseFile(const char* filepath)
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

static uint32_t CompileShader(const char* source, uint32_t type)
{
	uint32_t id = glCreateShader(type);
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

static uint32_t linkProgram(uint32_t vertexShader, uint32_t fragmentShader)
{
	uint32_t program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	int linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		int length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		char* message = alloca(length);
		glGetProgramInfoLog(program, length, &length, message);
		REYNOLDS_ERROR("@shader: link failed: %s", message);
		glDeleteProgram(program);
		return 0;
	}
	
	return program;
}

uint32_t CreateShader(char* vertexSource, char* fragmentSource)
{
	uint32_t vs 		= CompileShader(vertexSource, GL_VERTEX_SHADER);
	uint32_t fs 		= CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
	uint32_t program 	= linkProgram(vs, fs);
	
	glDeleteShader(vs);
	glDeleteShader(fs);
	
	free(vertexSource);
	free(fragmentSource);
	
	return program;
}

struct Shader* LoadShader(const char* filepath)
{
	ShaderProgramSource source = parseFile(filepath);

	if(!source.VertexSource || !source.FragmentSource)
	{
		REYNOLDS_ERROR("@shader: Failed to load shader from path: %s", filepath);
		return NULL;
	}
	
	uint32_t programID = CreateShader(source.VertexSource, source.FragmentSource);
	
	struct Shader* shader = malloc(sizeof(struct Shader));
	shader -> programID = programID;
	shader -> uniforms = NULL;

	return shader;
}

void BindShader(struct Shader* shader)
{
	if (shader -> programID == currently_bound) return;
	glUseProgram(shader -> programID);
	currently_bound = shader -> programID;
}

void UnbindShader()
{
	glUseProgram(0);
	currently_bound = 0;
}

void FreeShader(struct Shader* shader)
{
	if(!shader) return;
	
	if(shader -> programID)
		glDeleteProgram(shader -> programID);
	
	if(shader -> uniforms)
		free_cache(shader -> uniforms);
	
	free(shader);
}

//finds uniform in shader and caches value for quick access next call

static int GetUniformLocation(struct Shader* shader, const char* name)
{
	int location, cached_location;

	if(!shader -> uniforms) shader -> uniforms = create_cache(CACHE_LIMIT);

	cached_location = cache_get(shader -> uniforms, name);
	if (cached_location != -1)
		return cached_location;

	location = glGetUniformLocation(shader -> programID, name);
	if (location == -1)
		REYNOLDS_ERROR("@shader: uniform %s does not exist!", name);
	
	cache_set(shader -> uniforms, name, location);
	return location;
}

void SetIntArray(struct Shader* shader, const char* name, int* array, uint32_t count)
{
	int location = GetUniformLocation(shader, name);
	glProgramUniform1iv(shader -> programID, location, count, array);
}

void setMat4(struct Shader* shader, const char* name, mat4 array)
{
	int location = GetUniformLocation(shader, name);
	glProgramUniformMatrix4fv(shader -> programID, location, 1, GL_FALSE, (float* )array);
}