#include "renderer/texture.h"

#include "stb_image.h"
#include "utils/vector.h"

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
	uint32_t width, height;
	uint32_t textureID;
};

struct Texture texture;
Vector AssetManager;

static float sprite_coords[4]; //UNCLEAR: is this ok since we using ptr to access and set vals then passing this onto the application
static vec2 cell_size = {0, 0};

//probs not the best name

void init_asset_manager()
{
	 vector_init(&AssetManager, sizeof(struct Texture), INITIAL_CAPACITY);
}

void create_texture(uint32_t white_texture)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &white_texture);
	glBindTexture(GL_TEXTURE_2D, white_texture);
}

void setWhiteTexture(uint32_t colour)
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colour);
}

uint32_t load_texture(const char* path, uint32_t filter, uint32_t wrap)
{
	int width, height, BPP;
	
	stbi_set_flip_vertically_on_load(true);
		
	uint32_t textureID;
	unsigned char* LocalBuffer = stbi_load(path, &width, &height, &BPP, STBI_rgb_alpha);
	
	ASSERT_LOG(LocalBuffer,  "@textures: Failed to load texture %s", stbi_failure_reason());
	
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
	
	texture.textureID = textureID;
	texture.width = width;
	texture.height = height;

	vector_push_back(&AssetManager, &texture);
	return textureID;
}

static void getTextureSize(uint32_t textureID, float* size)
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

//iterating over a 2d array put in notes
static void createSubTexture(vec2 inf, vec2 sup)
{
	float* ptr = &sprite_coords[0];
	
	*ptr++ 	= inf[0];
	*ptr++ 	= inf[1];
	*ptr++ 	= sup[0];
	*ptr 	= sup[1];
}

//TO-DO: caching this info later?
void* createTexCoords(uint32_t textureID, vec2 sample_pos, vec2 sprite_size)
{
	float texture_size[2];
	getTextureSize(textureID, texture_size);

	float width 	= (float)texture_size[0];
	float height 	= (float)texture_size[1];

	float xpos 			= (float)sample_pos[0];
	float ypos 			= (float)sample_pos[1];
	float cell_width 	= (float)cell_size[0];
	float cell_height 	= (float)cell_size[1];
	float sprite_width 	= (float)sprite_size[0];
	float sprite_height = (float)sprite_size[1];
	
	if(!(cell_width && cell_height))
	{
		REYNOLDS_WARN("@texture: subtexture cell size not set.");
		return NULL;
	}

	vec2 inf = { (xpos * cell_width) / width , (ypos * cell_height) / height };
	vec2 sup = { ((xpos + sprite_width) * cell_width) / width , ((ypos + sprite_height) * cell_height) / height };
	
	createSubTexture(inf, sup);
	return sprite_coords;
}

void setCellSize(float width, float height)
{
	cell_size[0] = width; 
	cell_size[1] = height;
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
}