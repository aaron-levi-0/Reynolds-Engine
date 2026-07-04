#ifndef APP_INPUT_H
#define APP_INPUT_H

#include "gamestate.h"

extern double Mxpos, Mypos;

#define IDX(x, y) ms_index(dc, x, y)

extern void getGridIndices(DisplayContext* dc, double mouse_px_x, double mouse_px_y, int* grid_coords);
extern void KeyInput(GameState* s);
extern void MouseInput(DisplayContext* dc, GameState* s);
extern uint32_t ms_index(DisplayContext* dc, int x, int y);

#endif