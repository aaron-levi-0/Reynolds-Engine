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

	struct Statistics stats;
};

#define PUSH_ELEMENT(name, type) push_struct_element(#name, type, false, offsetof(Vertex, name))

#endif
