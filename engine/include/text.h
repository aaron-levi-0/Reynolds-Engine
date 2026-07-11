#ifndef TEXT_H
#define TEXT_H

struct Font;

REN_API struct Font* LoadFont(const char* ttf_path, float font_size);

#endif //TEXT_H