#ifndef APP_INPUT_H
#define APP_INPUT_H

#include "gamestate.h"

extern void getGridIndices(GameState* , double , double, int* );
extern void KeyInput(GameState*);
extern void MouseInput(GameState* );

#endif