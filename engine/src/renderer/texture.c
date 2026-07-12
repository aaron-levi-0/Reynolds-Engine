#include "renderer/texture_internals.h"

#include <GL/glew.h>

#include <stdint.h>
#include <stdlib.h>

#include "stb_image.h"
#include "utils/vector.h"
#include "utils/hash.h"

/* typedef struct {

	union { 
		vec4 full_sample; 	
		
		struct { 
			uint32_t sample_width, sample_height; 
			uint32_t xpos, ypos; 
		};
	};
} SpriteSheet;
 */

#define INITIAL_CAPACITY	4
#define GROWTH_RATE			2

struct Texture{
	uint32_t width, height; //TO-DO: union- texture.width or subtexture.cell_height
	uint32_t textureID;
};

Vector AssetManager; //probs not the best name
bool manager_ready = false;

void init_asset_manager()
{
	if(manager_ready) return;
	vector_init(&AssetManager, sizeof(struct Texture), INITIAL_CAPACITY);
	manager_ready = true;
}

void create_texture(uint32_t* white_texture)
{
	glCreateTextures(GL_TEXTURE_2D, 1, white_texture);
	glBindTexture(GL_TEXTURE_2D, *white_texture);
}

void setWhiteTexture(uint32_t colour)
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colour);
}

static uint32_t load_texture(const char* path, uint32_t filter, uint32_t wrap)
{
	int width, height, BPP;
	init_asset_manager();

	stbi_set_flip_vertically_on_load(true);
		
	uint32_t textureID;
	unsigned char* LocalBuffer = stbi_load(path, &width, &height, &BPP, STBI_rgb_alpha);
	
	ASSERT_FATAL(LocalBuffer,  "@textures: Failed to load texture %s", stbi_failure_reason());
	
	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);	
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, LocalBuffer);
	
	if(LocalBuffer)
		stbi_image_free(LocalBuffer);
	
	REYNOLDS_INFO("@textures: loaded successfully (width: %d, height: %d)", width, height);
	
	struct Texture texture;
	texture.textureID = textureID;
	texture.width = width;
	texture.height = height;

	vector_push_back(&AssetManager, &texture);
	return textureID;
}

uint32_t LoadTexture(const char* filepath)
{
	return load_texture(filepath, GL_LINEAR, GL_REPEAT);
}

void getTextureSize(uint32_t textureID, uint32_t* size)
{
	struct Texture* manager = (struct Texture* )vector_at(&AssetManager, 0);

	for (uint32_t i = 0; i < vector_size(&AssetManager); i++)
	{
		if(manager -> textureID == textureID)
		{
			size[0] = manager -> width;
			size[1] = manager -> height;
			return;
		}
		manager++;
	}
	memset(size, 0, sizeof(vec2));
	
	REYNOLDS_WARN("@texture: No texture of id: %u was found.", textureID);
	return;
}

uint32_t CreateTextureFromPixels(const uint8_t* pixels, uint32_t width, uint32_t height)
{
	ASSERT_FATAL(pixels, "@textures: Texture given NULL pixels.");
	init_asset_manager();

	uint32_t textureID;
	glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	struct Texture texture = { .textureID = textureID, .width = width, .height = height };
	vector_push_back(&AssetManager, &texture);

	return textureID;
}

void freeTextures()
{
	struct Texture* manager = (struct Texture*)AssetManager.data;
	for (uint32_t i = 0; i < vector_size(&AssetManager); i++)
	{
		glDeleteTextures(1, &manager -> textureID);
		manager++;
	}

	free_vector(&AssetManager);
	manager_ready = false; //TO-DO: do only when free_vector is successful
}