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

void SetShaderPath(struct Renderer* r, const char* path)
{
	uint32_t path_str_len = strlen(path);
	r -> ShaderPath = malloc(path_str_len + 1);
	strncpy(r -> ShaderPath, path, path_str_len + 1);
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
	
	//load uniform textures
	int samplers[MAX_TEXTURE_SLOTS];
	for (int i = 0; i < MAX_TEXTURE_SLOTS; i++)
		samplers[i] = i;
	
	/* Load shaders and use the resulting shader program */
	ShaderProgramSource source = parseFile(renderer -> ShaderPath);
	unsigned int program = CreateShader(source.VertexSource, source.FragmentSource);
	bind_shader(program);
	
	setIntArray("u_textures", samplers, MAX_TEXTURE_SLOTS);
}

static void FreeDraw(struct Renderer* renderer)
{
	/* Deletes all buffers and buffer pointers. */
	
	deleteVA(renderer -> QuadVA);
	freeBuffer(renderer -> QuadVB);
	freeBuffer(renderer -> QuadIB);
	
	glDeleteTextures(1, &renderer -> WhiteTexture);
	
	if(renderer -> ShaderPath)
		free(renderer -> ShaderPath);
	if(renderer -> QuadBuffer)
		free(renderer -> QuadBuffer);
}

// Function to handle resize events
static void onEvent(Event* e) 
{
    if (e->type == WindowResize) 
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
	for (unsigned int i = 0; i < renderer -> TextureSlotIndex; i++)
		glBindTextureUnit(i, renderer -> texture_slots[i]);
	
	drawTriangles(renderer -> QuadVA, renderer -> IndexCount);
	
	renderer -> IndexCount 			= 0;
	renderer -> TextureSlotIndex 	= 1;
	renderer -> stats.DrawCalls++;
}

//unable to pass by reference, position loses structure? also note texcoords structure is {(x,y)_beginning, (x,y)_end} // can use tex size instead?
void DrawQuad(struct Renderer* renderer, vec2 position, vec2 size, unsigned int textureID, vec4 tex_coords) 
{
    if (renderer->IndexCount >= MAX_INDICIES || renderer -> TextureSlotIndex > MAX_TEXTURE_SLOTS - 1) 
	{
        EndBatch(renderer);
        FlushBatch(renderer);
        BeginBatch(renderer);
    }

    unsigned int texture_index = 0; //white texture
	
	//check if renderer already has the texture loaded
	for (unsigned int i = 1; i < renderer->TextureSlotIndex; i++) 
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
		texture_index = renderer->TextureSlotIndex;
		renderer -> texture_slots[texture_index] = textureID;
		renderer -> TextureSlotIndex++;
	}
    
	/** 
		Note to self, study pointers deeper. Creating a local Vertex pointer means that 
		dereferencing of renderer only happens once instead of many times. **/
    Vertex* QuadBufferPtr = renderer -> QuadBufferPtr;
	
	/* load data into renderer */
	
    //vertex 1
	QuadBufferPtr -> position[0] 		= position[0];
	QuadBufferPtr -> position[1] 		= position[1];
	QuadBufferPtr -> texture_coord[0] 	= tex_coords[0];
	QuadBufferPtr -> texture_coord[1] 	= tex_coords[1];
	QuadBufferPtr -> textureID 			= (float) texture_index;
	QuadBufferPtr++;
	
	//vertex 2
	QuadBufferPtr -> position[0] 		= position[0] + size[0];
	QuadBufferPtr -> position[1] 		= position[1];
	QuadBufferPtr -> texture_coord[0] 	= tex_coords[2];
	QuadBufferPtr -> texture_coord[1] 	= tex_coords[1];
	QuadBufferPtr -> textureID 			= (float) texture_index;
	QuadBufferPtr++;
	
	//vertex 3
	QuadBufferPtr -> position[0] 		= position[0] + size[0];
	QuadBufferPtr -> position[1] 		= position[1] + size[1];
	QuadBufferPtr -> texture_coord[0] 	= tex_coords[2];
	QuadBufferPtr -> texture_coord[1] 	= tex_coords[3];
	QuadBufferPtr -> textureID 			= (float) texture_index;
	QuadBufferPtr++;
	
	//vertex 4
	QuadBufferPtr -> position[0] 		= position[0];
	QuadBufferPtr -> position[1] 		= position[1] + size[1];
	QuadBufferPtr -> texture_coord[0] 	= tex_coords[0];
	QuadBufferPtr -> texture_coord[1] 	= tex_coords[3];
	QuadBufferPtr -> textureID 			= (float) texture_index;
	QuadBufferPtr++;
	
	//update location of renderer quad pointer and increase index count
    renderer -> QuadBufferPtr = QuadBufferPtr;
    renderer -> IndexCount += 6;
	
	renderer -> stats.QuadCount++;
}

void DrawColour(struct Renderer* renderer, vec2 position, vec2 size, vec4 colour) 
{
    if (renderer -> IndexCount >= MAX_INDICIES) 
	{
        EndBatch(renderer);
        FlushBatch(renderer);
        BeginBatch(renderer);
    }

    unsigned int texture_index = 0; //white texture

    Vertex* QuadBufferPtr = renderer -> QuadBufferPtr;

    for (int i = 0; i < 4; i++) 
	{
        QuadBufferPtr -> position[0] = position[0] + (i == 1 || i == 2 ? size[0] : 0);
        QuadBufferPtr -> position[1] = position[1] + (i >= 2 ? size[1] : 0);
        QuadBufferPtr -> colour[0] = colour[0];
        QuadBufferPtr -> colour[1] = colour[1];
        QuadBufferPtr -> colour[2] = colour[2];
        QuadBufferPtr -> colour[3] = colour[3];
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
