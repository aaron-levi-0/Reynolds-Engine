#ifndef GAMEPLAY_LAYER_H
#define GAMEPLAY_LAYER_H

#include "core/layers.h"
#include "gamestate.h"

Layer create_gameplay_layer(struct Renderer* r, DisplayContext* dc, GameState* s);

#endif // GAMEPLAY_LAYER_H