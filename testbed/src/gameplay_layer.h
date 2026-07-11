#ifndef GAMEPLAY_LAYER_H
#define GAMEPLAY_LAYER_H

#include <layers.h>
#include "gamestate.h"
#include "text.h"

extern Layer create_gameplay_layer(struct Renderer* r, DisplayContext* dc, GameState* s);
extern void setFont(struct Font* f);

#endif // GAMEPLAY_LAYER_H