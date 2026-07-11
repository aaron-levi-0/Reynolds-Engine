#include "text_internals.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "stb_truetype.h"
#include "logging.h"
#include "renderer/texture_internals.h"

//TO-DO: move to utils/file.c — the map data files will want this too.
static uint8_t* read_entire_file(const char* path, size_t* out_size)
{
	FILE* fp = fopen(path, "rb");
	if (!fp)
	{
		REYNOLDS_ERROR("@text: cannot open file: %s", path);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	uint8_t* data = malloc(size);
	if (!data || fread(data, 1, size, fp) != (size_t)size)
	{
		REYNOLDS_ERROR("@text: failed reading file: %s", path);
		free(data);
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	if (out_size) *out_size = size;
	return data;
}


struct Font* LoadFont(const char* ttf_path, float font_size)
{
	size_t   ttf_size;
	uint8_t* ttf = read_entire_file(ttf_path, &ttf_size);
	if (!ttf) return NULL;

	struct Font* font = malloc(sizeof(struct Font));
	if (!font) { free(ttf); return NULL; }
	font -> font_size = font_size;

	/* Rasterise the glyph outlines into a single-channel coverage bitmap (CPU). */
	uint8_t* coverage = malloc(ATLAS_SIZE * ATLAS_SIZE);
	if (!coverage) { free(ttf); free(font); return NULL; }

	int baked = stbtt_BakeFontBitmap(ttf, 0, font_size, coverage,
	                                 ATLAS_SIZE, ATLAS_SIZE,
	                                 FIRST_CHAR, NUM_CHARS, font -> chars);
	free(ttf);								// outlines no longer needed once baked

	if (baked <= 0)							// negative: only -baked glyphs fit
	{
		REYNOLDS_ERROR("@text: atlas too small for %s at %.0fpx (fit %d glyphs)",
		               ttf_path, font_size, -baked);
		free(coverage);
		free(font);
		return NULL;
	}

    /* Expand 1 byte/pixel coverage to RGBA: white text, coverage as alpha —
	   flows through the existing RGBA8 pipeline and tints via v_colour. */
	uint8_t* rgba = malloc(ATLAS_SIZE * ATLAS_SIZE * 4);
	if (!rgba) { free(coverage); free(font); return NULL; }

	for (size_t i = 0; i < ATLAS_SIZE * ATLAS_SIZE; i++)
	{
		rgba[i * 4 + 0] = 255;
		rgba[i * 4 + 1] = 255;
		rgba[i * 4 + 2] = 255;
		rgba[i * 4 + 3] = coverage[i];
	}
	free(coverage);

	font -> atlasID = rasterize(rgba, ATLAS_SIZE, ATLAS_SIZE);
	free(rgba);								// GPU has its own copy now

	REYNOLDS_INFO("@text: loaded %s at %.0fpx", ttf_path, font_size);
	return font;
}

void FreeFont(struct Font* font)
{
	if (!font) return;
	/* atlas texture is tracked by the asset manager and freed with freeTextures() */
	free(font);
}

float TextWidth(struct Font* font, const char* text, float size)
{
	if (!font || !text) return 0.0f;

	float xpos = 0.0f, ypos = 0.0f;
	stbtt_aligned_quad q;

	for (const char* c = text; *c; c++)
	{
		if (*c < FIRST_CHAR || *c >= FIRST_CHAR + NUM_CHARS) continue;
		stbtt_GetBakedQuad(font -> chars, ATLAS_SIZE, ATLAS_SIZE,
		                   *c - FIRST_CHAR, &xpos, &ypos, &q, 1);
	}

	return xpos * (size / font -> font_size);	// pen's final advance = string width
}