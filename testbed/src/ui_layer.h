#ifndef UI_LAYER_H
#define UI_LAYER_H

#include <layers.h>
#include <ui.h>
#include "gamestate.h"

Layer create_ui_layer(struct Renderer* r, DisplayContext* dc, GameState* s, struct UIContext* ui);
#endif // UI_LAYER_H