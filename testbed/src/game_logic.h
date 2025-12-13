#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>
/* Handles the core game logic, such as win/loss conditions and player actions. */

//variables
extern bool win;
extern bool lose;

//functions
extern int valid_tile(int , int );
extern void reveal_tile(uint8_t , uint8_t );
extern bool valid_flag(int, int );
extern bool check_win();

#endif