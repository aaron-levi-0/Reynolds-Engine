#ifndef TEXT_H
#define TEXT_H

#include "defines.h"

struct Font;

REN_API struct Font* LoadFont(const char* ttf_path, float font_size);
REN_API void FreeFont(struct Font* font);
REN_API float TextWidth(struct Font* font, const char* text, float size);

#endif //TEXT_H