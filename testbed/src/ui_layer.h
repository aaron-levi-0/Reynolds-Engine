#ifndef UI_LAYER_H
#define UI_LAYER_H

#include <layers.h>
#include "gamestate.h"

Layer create_ui_layer(struct Renderer* r, DisplayContext* dc, GameState* s);

#endif // UI_LAYER_H