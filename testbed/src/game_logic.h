#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>
#include "gamestate.h"
/* Handles the core game logic, such as win/loss conditions and player actions. */

//variables
extern bool win;
extern bool lose;

//functions
extern void reveal_tile(DisplayContext* dc, GameState* s, int x, int y);
extern bool valid_flag(DisplayContext* dc, GameState* s, int x, int y);
extern bool check_win(DisplayContext* dc, GameState* s);

#endif