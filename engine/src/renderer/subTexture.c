#include "renderer/subtexture.h"

#include <stdlib.h>
#include <string.h>

#include "renderer/texture.h"
#include <cglm/cglm.h>

void subtex_init(Vector* entries, size_t initial_capacity)
{
    vector_init(entries, sizeof(SubTexture), initial_capacity);
}

void subtex_shutdown(Vector* entries)
{
    free_vector(entries);
}

static int subtex_find_index(const Vector* entries, uint32_t textureID, uint16_t cell_location[2])
{
    SubTexture* st = NULL;

    uint16_t col = cell_location[0];
    uint16_t row = cell_location[1];

    for (uint32_t i = 0; i < entries -> size; i++)
    {
        st = (SubTexture*)vector_at(entries, i);

        if (st -> textureID == textureID && st -> cell_location[0] == col && st -> cell_location[1] == row)
            return (int)i;
    }

    return -1;
}

static SubTexture* get_subtex_struc(const Vector* entries, uint32_t textureID, uint16_t cell_location[2])
{
    SubTexture* st = NULL;

    int idx = subtex_find_index(entries, textureID, cell_location);
    if (idx < 0) return NULL;

    st = (SubTexture*)vector_at(entries, (size_t) idx);
    return st;
}

float* get_uv(const Vector* entries, uint32_t textureID, uint16_t cell_location[2])
{
    ASSERT_LOG(entries, "get_uv: invalid args");
    SubTexture* st = get_subtex_struc(entries, textureID, cell_location);
    return st ? st -> uv : NULL;
}

static void cell_to_px(uint32_t textureID, uint16_t sample_pos[2], vec2 sprite_size, vec2 cell_size, vec4 out_vec)
{
	uint32_t texture_size[2];
	getTextureSize(textureID, texture_size);

	float sheet_width 	= (float)texture_size[0];
	float sheet_height 	= (float)texture_size[1];

	float xpos 			= (float)sample_pos[0];
	float ypos 			= (float)sample_pos[1];
	float sprite_width 	= (float)sprite_size[0];
	float sprite_height = (float)sprite_size[1];
	float cell_width 	= (float)cell_size[0];
	float cell_height 	= (float)cell_size[1];
	
	if(!(cell_width && cell_height))
	{
		REYNOLDS_WARN("@texture: subtexture cell size not set.");
		return;
	}

	vec2 u = { (xpos * cell_width) / sheet_width , ((xpos + sprite_width) * cell_width) / sheet_width };
	vec2 v = { (ypos * cell_height) / sheet_height, ((ypos + sprite_height) * cell_height) / sheet_height };
	
	glm_vec4_copy((vec4){u[0], v[0], u[1], v[1]}, out_vec);
}

bool subtex_register_cell(Vector* reg,
                        uint32_t textureID,
                        uint16_t cell_pos[2],
                        vec2 cell_size,
                        vec2 sample_size)
{
    ASSERT_LOG(reg && textureID > 0, "subtex_register_cell: invalid args");

    uint16_t col = (uint16_t) cell_pos[0];  
    uint16_t row = (uint16_t) cell_pos[1];

    // Disallow duplicates (or you can choose to overwrite)
    if (subtex_find_index(reg, textureID, (uint16_t[]) {col, row}) >= 0)
        return true;

    SubTexture st = {0};
    st.textureID = textureID;
    st.cell_location[0] = col;
    st.cell_location[1] = row;
    cell_to_px(textureID, cell_pos, sample_size, cell_size, st.uv);
    vector_push_back(reg, &st);

    return true;
}

//subtex_register_cell(texture_register, "TILE_CLOSED", sprite_map, sprite[TILE_CLOSED], SPRITE_CELL_SIZE, sprite_size)

// bool subtex_register_px(Vector* reg,
//                         uint32_t textureID,
//                         vec2 pixel_pos,
//                         vec2 pixel_size)
// {
//     ASSERT_LOG(reg, "subtex_register_px: invalid args");

//     // Disallow duplicates (or you can choose to overwrite)
//     if (subtex_find_index(reg) >= 0)
//     {
//         //REYNOLDS_VERBOSE("sub-texure: '%s' obtained from cache", name); <- prints every frame!
//         return false;
//     }

//     uint32_t texture_size[2];
// 	getTextureSize(textureID, texture_size);

//     uint32_t sheet_width 	= texture_size[0];
//     uint32_t sheet_height 	= texture_size[1];

//     ASSERT_LOG(sheet_width > 0 && sheet_height > 0, "subtex_register_px: invalid sheet dims");

//     // Convert pixels -> normalized UV.
//     // NOTE: coordinate origin depends on how your textures are authored + how stb_image is flipped.
//     // This assumes (0,0) is TOP-LEFT in pixel space. If yours is bottom-left, flip v.
//     float u0 = pixel_pos[0] / (float)sheet_width;
//     float v0 = pixel_pos[1] / (float)sheet_height;
//     float u1 = (pixel_pos[0] + pixel_size[0]) / (float)sheet_width;
//     float v1 = (pixel_pos[1] + pixel_size[1]) / (float)sheet_height;

//     SubTexture st = {0};

//     st.textureID = textureID;
//     glm_vec4_copy((vec4){u0, v0, u1, v1}, st.uv);
//     vector_push_back(reg, &st); //func pushes back a copy of the struct

//     return true;
// }
    //subtex_register_px(&atlas, "closed", board_pieces, (vec2){100,235}, (vec2){10,10});