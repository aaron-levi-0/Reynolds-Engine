#ifndef TEXT_INTERNALS_H
#define TEXT_INTERNALS_H

#include <stdint.h>
#include "stb_truetype.h"
#include "text.h"

#define ATLAS_SIZE   512		// 512x512 fits ASCII comfortably up to ~48px
#define FIRST_CHAR   32			// ' ' — first printable ASCII
#define NUM_CHARS    96			// through '~' (32 + 96 - 1 = 127 exclusive)

struct Font
{
	uint32_t         atlasID;				// GPU texture handle
	float            font_size;				// size the glyphs were baked at
	stbtt_bakedchar  chars[NUM_CHARS];		// per-glyph atlas rects + metrics
};

#endif //TEXT_INTERNALS_H
