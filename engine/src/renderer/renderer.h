#ifndef RENDERER_H
#define RENDERER_H

#include "core/glx.h"
#include "core/layers.h"
#include "renderer/shader.h"

#define	MAX_TEXTURE_SLOTS	32
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
};

struct Statistics
{
	uint32_t DrawCalls;
	uint32_t QuadCount;
};

#define PUSH_ELEMENT(name, type) push_struct_element(#name, type, false, offsetof(Vertex, name))

extern void setClearColour(vec3 );
extern void render_clear();
REN_API Layer create_render_layer();

void setShaderPath(const char* );

extern void render_init(struct Renderer* );

extern void MallocDraw(struct Renderer* );
extern void FreeDraw(struct Renderer* );

extern void BeginBatch(struct Renderer* );
extern void EndBatch(struct Renderer* );
extern void FlushBatch(struct Renderer* );

extern void DrawQuad(struct Renderer* , vec2 , vec2 , unsigned int , vec4 );
extern void DrawColour(struct Renderer* , vec2 , vec2 , vec4 );

extern struct Statistics getRenderStats();
extern void resetStats();

#endif
