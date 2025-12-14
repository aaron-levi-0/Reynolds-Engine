#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>
#include "gamestate.h"
/* Handles the core game logic, such as win/loss conditions and player actions. */

//variables
extern bool win;
extern bool lose;

//functions
extern void reveal_tile(GameState* , int , int );
extern bool valid_flag(GameState* , int , int );
extern bool check_win(GameState*);

#endif