#ifndef BOARD_H
#define BOARD_H

#include "gamestate.h"

//#define BOARD_WIDTH		1054.0f	// board width (in pixels)
//#define GRID_WIDTH		1000.0f	// width of game instance (in pixels)
//#define SIZE 20
//#define BOMBS 40 //make these customisable later

#define LINE_THICKNESS	4.0f 	// thickness of the grid lines (in pixels)
#define BOMB -1
#define EMPTY 0

/* Responsible for handling the game board's structure and operations. */

extern int  valid_tile(DisplayContext* dc, int x, int y);
extern void init_board_state(DisplayContext* dc, GameState* s, const uint32_t* config);
extern void reset_board(DisplayContext* dc, GameState* s);
extern void delete_board(GameState* s);
extern void place_mines(DisplayContext* dc, GameState* s);

#endif