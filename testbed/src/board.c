#include "board.h"

#include <string.h>
#include <logging.h>

#include "common.h"
#include "gamestate.h"
#include "app_input.h"

int valid_tile(DisplayContext* dc, int x, int y)
{
	/* Function to check bounds */
    return ((x >= 0 && x < (int) dc -> tiles_x) && (y >= 0 && y < (int) dc -> tiles_y));
}

void place_mines(DisplayContext* dc, GameState* s) 
{
	ASSERT_FATAL(s -> board, "Application trying to access non-existant board memory!");
	
	int x, y;
	Tile* board = s -> board;
	
    for (uint16_t i = 0; i < s -> num_bombs; i++) 
	{
         do {
			 
            x = rand() % dc -> tiles_x;	
            y = rand() % dc -> tiles_y;
			
        } while (board[IDX(x, y)].value == BOMB);	//y tiles are the height which are the rows
		
        board[IDX(x, y)].value = BOMB;
		REYNOLDS_VERBOSE("Placing mine at (%d, %d)", x, y);
		
        // Increment numbers around the bomb
        for (int dx = -1; dx <= 1; dx++) 
		{
            for (int dy = -1; dy <= 1; dy++) 
			{
                int nx = x + dx, ny = y + dy;
                if (valid_tile(dc, nx, ny) && board[IDX(nx, ny)].value != BOMB)
					board[IDX(nx, ny)].value++;
            }
        }
    }
}

void delete_board(Tile* board) 
{
	ASSERT_FATAL(board, "Attempting to free board memory which doesn't exist!");
	free(board);
}

void init_board_state(DisplayContext* dc, GameState* s, const uint32_t* config)
{
	if(s -> board)
		delete_board(s -> board);

	//TO-DO: need to check if len of configs[] is exactly 7*int?
	dc -> width       	= (uint16_t)config[0];
    dc -> height      	= (uint16_t)config[1];
    dc -> grid_width  	= (uint16_t)config[2];
    dc -> grid_height 	= (uint16_t)config[3];
    dc -> tiles_x     	= (uint8_t) config[4];
    dc -> tiles_y     	= (uint8_t) config[5];
    s -> num_bombs   	= (uint16_t)config[6];
		
	ASSERT_FATAL(dc -> width > 0, 		"board size is invalid!");
	ASSERT_FATAL(dc -> height > 0, 		"board size is invalid!");
	ASSERT_FATAL(dc -> grid_width > 0,	"grid size is invalid!");
	ASSERT_FATAL(dc -> grid_height > 0,	"grid size is invalid!");
	ASSERT_FATAL(dc -> tiles_x > 0, 	"number of tiles is invalid!");
	ASSERT_FATAL(dc -> tiles_y > 0, 	"number of tiles is invalid!");
	ASSERT_FATAL(s -> num_bombs > 0, 	"number of bombs is invalid!");

	const int rows = dc -> tiles_y;
    const int cols = dc -> tiles_x;
    const int total_cells = rows * cols;
	
	ASSERT_FATAL(s->num_bombs <= total_cells,"Number of bombs exceeds number of tiles");

	//set the contiguos board memory
	s -> board = calloc(total_cells, sizeof(Tile));
	ASSERT_FATAL(s -> board, "Failed to initialise board state!\n");
	
	place_mines(dc, s);
}

void reset_board(DisplayContext* dc, GameState* s) 
{
	// Clearing board state of bombs and numbers, closing all revealed tiles
	memset(s -> board, 0, dc -> tiles_y * dc -> tiles_x * sizeof(Tile));
  
	REYNOLDS_VERBOSE("Resetting board: %d rows, %d cols", dc -> tiles_y, dc -> tiles_x);
	place_mines(dc, s);
}