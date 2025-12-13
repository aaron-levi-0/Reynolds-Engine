#include "renderer/renderer.h"

#include "core/window.h"
#include "events/event.h"
#include "renderer/buffer.h"
#include "renderer/texture.h"

struct Statistics stats = {0};

char* SHADER_PATH = NULL;

void setShaderPath(const char* path)
{
	uint32_t path_str_len = strlen(path);
	SHADER_PATH = malloc(path_str_len + 1);
	strncpy(SHADER_PATH, path, path_str_len + 1);
}

void setClearColour(vec3 colour)
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

static void on_render_event(uint32_t width, uint32_t height)
{
	setViewPort(0, 0, width, height);
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
        on_render_event(width, height);
        REYNOLDS_VERBOSE("@render layer: Window resized to (%d, %d)", width, height);
    }
}

// Function to create the render layer
Layer create_render_layer() {
    Layer render_layer = {
        .name = "Base Render Layer",
		.id = RENDER,
        .onEvent = onEvent, // Handle resize events
    };
    return render_layer;
}

void render_init(struct Renderer* renderer)
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

void MallocDraw(struct Renderer* renderer)
{
	/*
		Creates and resereves a vertex buffer to the GPU with the attribute structure defined. Attributes
		are established in such a way that each buffer segment is read *per vertex* of a quad.
		
		Establishes an index buffer that repeats the simple quad pattern until it fills MAX_INDICIES.
		
		Creates a white 'texture' in texture slot 0 that acts as an empty texture. When multiplied with colour
		vectors, the colour vector is returned unchanged. Use this texture slot when you want to render an object
		without any texture samples.
	*/
	
	ASSERT_LOG(!renderer -> QuadBuffer, "@renderer: 'QuadBuffer' initialised twice.");
	
	size_t VertexSize = sizeof(Vertex);
	renderer -> QuadBuffer 	= malloc(VertexSize * MAX_VERTICIES);
	
	ASSERT_LOG(renderer -> QuadBuffer, "@renderer: Failed to allocate 'QuadBuffer' memory.");

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
	create_texture(renderer -> WhiteTexture);
	renderer -> texture_slots[0] = renderer -> WhiteTexture;
	setWhiteTexture(WHITE);
	
	//load uniform textures
	int samplers[MAX_TEXTURE_SLOTS];
	for (int i = 0; i < MAX_TEXTURE_SLOTS; i++)
		samplers[i] = i;
	
	/* Load shaders and use the resulting shader program */
	ShaderProgramSource source = parseFile(SHADER_PATH);
	unsigned int program = CreateShader(source.VertexSource, source.FragmentSource);
	bind_shader(program);
	
	setIntArray("u_textures", samplers, MAX_TEXTURE_SLOTS);
}

void FreeDraw(struct Renderer* renderer)
{
	/* Deletes all buffers and buffer pointers. */
	
	deleteVA(renderer -> QuadVA);
	freeBuffer(renderer -> QuadVB);
	freeBuffer(renderer -> QuadIB);
	
	glDeleteTextures(1, &renderer -> WhiteTexture);
	
	if(SHADER_PATH)
		free(SHADER_PATH);
	if(renderer -> QuadBuffer)
		free(renderer -> QuadBuffer);
}

void BeginBatch(struct Renderer* renderer)
{
	ASSERT(renderer -> QuadBuffer, "in renderer.c: failed to find 'QuadBuffer'");
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
	stats.DrawCalls++;
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
	
	stats.QuadCount++;
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
	
	stats.QuadCount++;
}

struct Statistics getRenderStats()
{
	return stats;
}

void resetStats()
{
	memset(&stats, 0, sizeof(struct Statistics));
}