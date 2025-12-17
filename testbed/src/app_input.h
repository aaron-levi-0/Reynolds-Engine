#ifndef APP_INPUT_H
#define APP_INPUT_H

#include "gamestate.h"

extern void getGridIndices(DisplayContext* dc, double mouse_px_x, double mouse_px_y, int* grid_coords);
extern void KeyInput(GameState* s);
extern void MouseInput(DisplayContext* dc, GameState* s);

#endif