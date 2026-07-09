#include "renderer/renderer_internals.h"

#include <GL/glew.h>

#include "core/window_internals.h"
#include "events/event.h"
#include "renderer/buffer.h"
#include "renderer/shader_internals.h"

struct Renderer* renderer_create() 
{
    struct Renderer* r = malloc(sizeof(struct Renderer));
    memset(r, 0, sizeof(*r));
    return r;
}

void renderer_destroy(struct Renderer* r) 
{
    if (!r) return;
    free(r);
}

void SetShader(struct Renderer* r, struct Shader* shader)
{
	r -> shader = shader;
}

void SetClearColour(vec3 colour)
{
	glClearColor(colour[0], colour[1], colour[2], 1.0);
}

void render_clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void setViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) 
{ 
	glViewport(x, y, width, height);
}

static void render_init(struct Renderer* renderer)
{
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	renderer -> QuadVA = 0;
	renderer -> QuadVB = 0;
	renderer -> QuadIB = 0;
	
	renderer -> WhiteTexture 		= 0;
	renderer -> WhiteTextureSlot 	= 0;
	
	renderer -> IndexCount = 0;

	renderer -> QuadBuffer 		= NULL;
	renderer -> QuadBufferPtr 	= NULL;
	
	renderer -> TextureSlotIndex = 1;
	
	for (size_t i = 0; i < MAX_TEXTURE_SLOTS; i++)
		renderer -> texture_slots[i] = 0;
}

static void MallocDraw(struct Renderer* renderer)
{
	/*
		Creates and resereves a vertex buffer to the GPU with the attribute structure defined. Attributes
		are established in such a way that each buffer segment is read *per vertex* of a quad.
		
		Establishes an index buffer that repeats the simple quad pattern until it fills MAX_INDICIES.
		
		Creates a white 'texture' in texture slot 0 that acts as an empty texture. When multiplied with colour
		vectors, the colour vector is returned unchanged. Use this texture slot when you want to render an object
		without any texture samples.
	*/
	
	ASSERT_FATAL(!renderer -> QuadBuffer, "@renderer: 'QuadBuffer' initialised twice.");
	
	size_t VertexSize = sizeof(Vertex);
	renderer -> QuadBuffer 	= malloc(VertexSize * MAX_VERTICIES);
	
	ASSERT_FATAL(renderer -> QuadBuffer, "@renderer: Failed to allocate 'QuadBuffer' memory.");

	/* Vertex Array and Vertex Buffer */
   	renderer -> QuadVA = createVA();
    renderer -> QuadVB = createVB(NULL, MAX_VERTICIES * VertexSize); 

	//Buffer structure
	create_buffer_layout();
	PUSH_ELEMENT(position, TYPE_FLOAT2);
	PUSH_ELEMENT(texture_coord, TYPE_FLOAT2);
	PUSH_ELEMENT(textureID, TYPE_FLOAT);
	PUSH_ELEMENT(colour, TYPE_FLOAT4);
	setBufferLayout(renderer -> QuadVA, VertexSize);
	delete_buffer_layout();

	/* Index Buffer */
	uint32_t indicies[MAX_INDICIES];
	size_t offset = 0;
	
	for (int i = 0; i < MAX_INDICIES; i+= 6)
	{
		indicies[i + 0] = 0 + offset;
		indicies[i + 1] = 1 + offset;
		indicies[i + 2] = 2 + offset;
		
		indicies[i + 3] = 2 + offset;
		indicies[i + 4] = 3 + offset;
		indicies[i + 5] = 0 + offset;
		
		offset += 4;
	}
	
   	renderer -> QuadIB = createIB(indicies, sizeof(indicies));

	/* Textures */
	
	//1x1 white texture
	init_asset_manager();
	create_texture(&renderer -> WhiteTexture);
	renderer -> texture_slots[0] = renderer -> WhiteTexture;
	setWhiteTexture(WHITE);
}

static void FreeDraw(struct Renderer* renderer)
{
	/* Deletes all buffers and buffer pointers. */
	
	deleteVA(renderer -> QuadVA);
	freeBuffer(renderer -> QuadVB);
	freeBuffer(renderer -> QuadIB);
	
	glDeleteTextures(1, &renderer -> WhiteTexture);
	
	if(renderer -> shader)
		FreeShader(renderer -> shader);
	if(renderer -> QuadBuffer)
		free(renderer -> QuadBuffer);
}

// Function to handle resize events
static void onEvent(Event* e) 
{
    if (e -> type == WindowResize) 
	{
        uint32_t width = getWindowWidth();
        uint32_t height = getWindowHeight();

        if (width == 0 || height == 0) 
		{
            // Window is minimized, skip rendering
            REYNOLDS_DEBUG("@render layer: Window minimized.");
            return;
        }

        // Update the renderer's viewport and projection
        setViewPort(0, 0, width, height);
        REYNOLDS_VERBOSE("@render layer: Window resized to (%d, %d)", width, height);
    }
}

static void onDetach(void* renderer)
{
	struct Renderer* r = (struct Renderer*) renderer;
	FreeDraw(r);
}

// Function to create the render layer
Layer create_render_layer(struct Renderer* r) 
{
	render_init(r);
	MallocDraw(r);
    Layer render_layer = {
        .name = "Render Layer",
		.id = LAYER_RENDER,
		.data = r,
		//.update = onUpdate,
        .onEvent = onEvent, // Handle resize events
		.onDetatch = onDetach
    };
    return render_layer;
}

void BeginBatch(struct Renderer* renderer)
{
	ASSERT_FATAL(renderer -> QuadBuffer, "in renderer.c: failed to find 'QuadBuffer'");
	renderer -> QuadBufferPtr = renderer -> QuadBuffer;
}

void EndBatch(struct Renderer* renderer)
{
	//size can be found since data is loaded using QuadBufferPtr (incrementing the pointer to load the next batch) not QuadBuffer itself
	ptrdiff_t size = (uint8_t*) renderer -> QuadBufferPtr - (uint8_t*) renderer -> QuadBuffer;
	writeVertexBuffer(renderer -> QuadVB, renderer -> QuadBuffer, size);
}

void FlushBatch(struct Renderer* renderer)
{
	BindShader(renderer -> shader);
	setMat4(renderer -> shader, "u_ProjectionView", renderer -> view_projection);

	for (uint32_t i = 0; i < renderer -> TextureSlotIndex; i++)
		glBindTextureUnit(i, renderer -> texture_slots[i]);
	
	drawTriangles(renderer -> QuadVA, renderer -> IndexCount);
	
	renderer -> IndexCount 			= 0;
	renderer -> TextureSlotIndex 	= 1;
	renderer -> stats.DrawCalls++;
}

void setViewProjection(struct Renderer* renderer, mat4 view_projection)
{
	ASSERT_FATAL(renderer, "@renderer: 'renderer' is NULL.");
	ASSERT_FATAL(view_projection, "@renderer: 'view_projection' is NULL.");
	
	glm_mat4_ucopy(view_projection, renderer -> view_projection);
}

static void setPosition(Vertex* vertex, const vec2 position)
{
	vertex -> position[0] = position[0];
	vertex -> position[1] = position[1];
}

static void setTexture(Vertex* vertex, const vec2 tex_coords, const uint32_t id)
{
	vertex -> texture_coord[0] = tex_coords[0];
	vertex -> texture_coord[1] = tex_coords[1];
	vertex -> textureID = (float) id;
}

static void setColour(Vertex* vertex, const vec4 colour)
{
	vertex -> colour[0] = colour[0];
	vertex -> colour[1] = colour[1];
	vertex -> colour[2] = colour[2];
	vertex -> colour[3] = colour[3];
}

//unable to pass by reference, position loses structure? also note texcoords structure is {(x,y)_beginning, (x,y)_end} // can use tex size instead?
void DrawQuad(struct Renderer* renderer, const vec2 position, const vec2 size, const uint32_t textureID, const vec4 tex_coords) 
{
	const vec4 white = {1.0f, 1.0f, 1.0f, 1.0f};

    if (renderer->IndexCount >= MAX_INDICIES || renderer -> TextureSlotIndex > MAX_TEXTURE_SLOTS - 1) 
	{
        EndBatch(renderer);
        FlushBatch(renderer);
        BeginBatch(renderer);
    }

    uint32_t texture_index = 0; //white texture
	
	//check if renderer already has the texture loaded
	for (uint32_t i = 1; i < renderer -> TextureSlotIndex; i++) 
	{
		if (renderer -> texture_slots[i] == textureID) 
		{
			texture_index = i;
			break;
		}
	}
	
	//place texture in renderer texture slot if it is not in already
	if (texture_index == 0) 
	{
		texture_index = renderer -> TextureSlotIndex;
		renderer -> texture_slots[texture_index] = textureID;
		renderer -> TextureSlotIndex++;
	}
    
	/** 
		Note to self, study pointers deeper. Creating a local Vertex pointer means that 
		dereferencing of renderer only happens once instead of many times. **/
    Vertex* QuadBufferPtr = renderer -> QuadBufferPtr;
	
	/* load data into renderer */
	
    //vertex 1
	setPosition(QuadBufferPtr, position);
	setTexture(QuadBufferPtr, (vec2){tex_coords[0], tex_coords[1]}, texture_index);
	setColour(QuadBufferPtr, white);
	QuadBufferPtr++;
	
	//vertex 2
	float position0 = position[0] + size[0];
	float position1 = position[1];

	setPosition(QuadBufferPtr, (vec2){position0, position1});
	setTexture(QuadBufferPtr, (vec2){tex_coords[2], tex_coords[1]}, texture_index);
	setColour(QuadBufferPtr, white);
	QuadBufferPtr++;
	
	//vertex 3
	position0 = position[0] + size[0];
	position1 = position[1] + size[1];

	setPosition(QuadBufferPtr, (vec2){position0, position1});
	setTexture(QuadBufferPtr, (vec2){tex_coords[2], tex_coords[3]}, texture_index);
	setColour(QuadBufferPtr, white);
	QuadBufferPtr++;
	
	//vertex 4
	position0 = position[0];
	position1 = position[1] + size[1];

	setPosition(QuadBufferPtr, (vec2){position0, position1});
	setTexture(QuadBufferPtr, (vec2){tex_coords[0], tex_coords[3]}, texture_index);
	setColour(QuadBufferPtr, white);
	QuadBufferPtr++;
	
	//update location of renderer quad pointer and increase index count
    renderer -> QuadBufferPtr = QuadBufferPtr;
    renderer -> IndexCount += 6;
	
	renderer -> stats.QuadCount++;
}

void DrawColour(struct Renderer* renderer, const vec2 position, const vec2 size, const vec4 colour) 
{
    if (renderer -> IndexCount >= MAX_INDICIES) 
	{
        EndBatch(renderer);
        FlushBatch(renderer);
        BeginBatch(renderer);
    }

    uint32_t texture_index = 0; //white texture

    Vertex* QuadBufferPtr = renderer -> QuadBufferPtr;

    for (int i = 0; i < 4; i++) 
	{
        QuadBufferPtr -> position[0] = position[0] + (i == 1 || i == 2 ? size[0] : 0);
        QuadBufferPtr -> position[1] = position[1] + (i >= 2 ? size[1] : 0);
        setColour(QuadBufferPtr, colour);
        QuadBufferPtr -> textureID = (float)texture_index; // texture index of 0 draws colour as stated in shader
        QuadBufferPtr++;
    }

    renderer -> QuadBufferPtr = QuadBufferPtr;
    renderer -> IndexCount += 6;
	
	renderer -> stats.QuadCount++;
}

void resetStats(struct Renderer* r) 
{
    if (!r) return;
    memset(&r -> stats, 0, sizeof(r -> stats));
}

uint32_t getDrawCalls(const struct Renderer* r) 
{
	if (!r) return 0;
	return r -> stats.DrawCalls;
}

uint32_t getQuadCount(const struct Renderer* r) 
{
	if (!r) return 0;
	return r -> stats.QuadCount;
}
