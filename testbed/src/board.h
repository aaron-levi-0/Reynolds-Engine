#ifndef BOARD_H
#define BOARD_H

#include <time.h>
#include "common.h"

//#define BOARD_WIDTH		1054.0f	// board width (in pixels)
//#define GRID_WIDTH		1000.0f	// width of game instance (in pixels)
//#define SIZE 20
//#define BOMBS 40 //make these customisable later

#define LINE_THICKNESS	4.0f 	// thickness of the grid lines (in pixels)
#define BOMB -1
#define EMPTY 0

//variables

typedef struct
{
	uint32_t width, height;
	uint32_t grid_width, grid_height;
	uint16_t num_bombs;
	uint8_t tiles_x, tiles_y;
	
	int** board;
	int** reveal_state;
} BOARD;

/* Responsible for handling the game board's structure and operations. */
extern int** 		getBoardState();
extern int** 		getRevealState();

extern uint16_t getBoardWidth();
extern uint16_t getBoardHeight();
extern uint16_t getGridWidth();
extern uint16_t getGridHeight();
extern uint8_t 	getBoardRows();
extern uint8_t 	getBoardCols();

extern int valid_tile(int , int );
extern void init_board_state(uint32_t* );
extern void calc_norms(vec2 , float** );
extern void reset_board();
extern void delete_board();
extern void place_mines();

#endif