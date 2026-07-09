#ifndef RENDERER_INTERNALS_H
#define RENDERER_INTERNALS_H

#include "renderer/texture_internals.h"
#include "renderer.h"
#include "core/glx.h"

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
	uint32_t QuadVA;
	uint32_t QuadVB;
	uint32_t QuadIB;

	uint32_t WhiteTexture;
	uint32_t WhiteTextureSlot;
	
	uint32_t IndexCount;

	Vertex* QuadBuffer;
	Vertex* QuadBufferPtr;
	
	uint32_t texture_slots[MAX_TEXTURE_SLOTS];
	uint32_t TextureSlotIndex;

	struct Shader* shader;

	mat4 view_projection;
	bool initialised;

	struct Statistics stats;
};

#define PUSH_ELEMENT(name, type) push_struct_element(#name, type, false, offsetof(Vertex, name))

extern void setViewProjection(struct Renderer* renderer, mat4 view_projection);
#endif
