#ifndef RENDERER_H
#define RENDERER_H

#include "core/glx.h"
#include "core/layers.h"
#include "renderer/shader.h"
#include "renderer/texture.h"

#define	MAX_QUADS			1000
#define	MAX_VERTICIES		4 * MAX_QUADS
#define	MAX_INDICIES		6 * MAX_QUADS
#define WHITE				0xffffffff

typedef struct 
{
	vec2 	position;
	vec2 	texture_coord;
	float	textureID;
	vec4 	colour;       // RGBA color
} Vertex;

struct Renderer
{
	unsigned int QuadVA;
	unsigned int QuadVB;
	unsigned int QuadIB;
	
	unsigned int WhiteTexture;
	unsigned int WhiteTextureSlot;
	
	unsigned int IndexCount;

	Vertex* QuadBuffer;
	Vertex* QuadBufferPtr;
	
	unsigned int texture_slots[MAX_TEXTURE_SLOTS];
	unsigned int TextureSlotIndex;

	bool initialised;
};

struct Statistics
{
	uint32_t DrawCalls;
	uint32_t QuadCount;
};

#define PUSH_ELEMENT(name, type) push_struct_element(#name, type, false, offsetof(Vertex, name))

REN_API void setClearColour(vec3 );
REN_API void render_clear();
REN_API Layer create_render_layer(struct Renderer* );

REN_API void setShaderPath(const char* );

REN_API void BeginBatch(struct Renderer* );
REN_API void EndBatch(struct Renderer* );
REN_API void FlushBatch(struct Renderer* );

REN_API void DrawQuad(struct Renderer* , vec2 , vec2 , unsigned int , vec4 );
REN_API void DrawColour(struct Renderer* , vec2 , vec2 , vec4 );

REN_API struct Statistics getRenderStats();
REN_API void resetStats();

#endif
