#include "board.h"

#include <string.h>
#include "common.h"
#include "gamestate.h"

int valid_tile(GameState* s,  int x, int y)
{
	/* Function to check bounds */
    return ((x >= 0 && x < (int) s -> tiles_x) && (y >= 0 && y < (int) s -> tiles_y));
}

void place_mines(GameState* s) 
{
	int x, y;
	
    for (uint16_t i = 0; i < s -> num_bombs; i++) 
	{
         do {
			 
            x = rand() % s -> tiles_x;	
            y = rand() % s -> tiles_y;
			
        } while (s -> board[y][x] == BOMB);	//y tiles are the height which are the rows
		
        s -> board[y][x] = BOMB;
		REYNOLDS_VERBOSE("Placing mine at (%d, %d)", x, y);
		
        // Increment numbers around the bomb
        for (int dx = -1; dx <= 1; dx++) 
		{
            for (int dy = -1; dy <= 1; dy++) 
			{
                int nx = x + dx, ny = y + dy;
                if (valid_tile(s, nx, ny) && s -> board[ny][nx] != BOMB)
					s -> board[ny][nx]++;
            }
        }
    }
}

void delete_board(GameState* s) 
{
	ASSERT_LOG(s -> board, "Attempting to free board memory which doesn't exist!");
	ASSERT_LOG(s -> reveal_state, "Attempting to free board memory which doesn't exist!");

	free(s -> board[0]);
	free(s -> reveal_state[0]);
	
	free(s -> board);
	free(s -> reveal_state);
	
	s -> board 			= NULL;
	s -> reveal_state 	= NULL;
}

void init_board_state(GameState* s, const uint32_t* config)
{
	if(s -> board || s -> reveal_state)
		delete_board(s);

	//TO-DO: need to check if len of configs[] is exactly 7*int?
	s -> width       = (uint16_t)config[0];
    s -> height      = (uint16_t)config[1];
    s -> grid_width  = (uint16_t)config[2];
    s -> grid_height = (uint16_t)config[3];
    s -> tiles_x     = (uint8_t) config[4];
    s -> tiles_y     = (uint8_t) config[5];
    s -> num_bombs   = (uint16_t)config[6];
		
	ASSERT_LOG(s -> width > 0, 		"board size is invalid!");
	ASSERT_LOG(s -> height > 0, 		"board size is invalid!");
	ASSERT_LOG(s -> grid_width > 0,	"grid size is invalid!");
	ASSERT_LOG(s -> grid_height > 0,	"grid size is invalid!");
	ASSERT_LOG(s -> tiles_x > 0, 		"number of tiles is invalid!");
	ASSERT_LOG(s -> tiles_y > 0, 		"number of tiles is invalid!");
	ASSERT_LOG(s -> num_bombs > 0, 	"number of bombs is invalid!");
	
	const int rows = s -> tiles_y;
    const int cols = s -> tiles_x;
    const int total_cells = rows * cols;
	
	//create row pointers
	int** board_rows 	= malloc(rows * sizeof(int*));
    int** reveal_state 	= malloc(rows * sizeof(int*));
	ASSERT_LOG(board_rows && reveal_state, "Failed to allocate memory for board!\n");

	//set the contiguos board memory
    board_rows[0]  	= calloc(total_cells, sizeof(int));
    reveal_state[0] = calloc(total_cells, sizeof(int));
	ASSERT_LOG(board_rows[0] && reveal_state[0], "Failed to initialise board state!\n");
	
	for (uint8_t i = 0; i < rows; i++)
	{	
		//set the row pointers
		board_rows[i] 	= board_rows[0] + i * cols;
		reveal_state[i] = reveal_state[0] + i * cols;
	}

	s -> board 			= board_rows;
	s -> reveal_state 	= reveal_state;

	place_mines(s);
}

void reset_board(GameState* s) 
{
	// Clearing board state of bombs and numbers, closing all revealed tiles
	memset(s -> board[0], 0, s -> tiles_y * s -> tiles_x * sizeof(int));
	memset(s -> reveal_state[0], 0, s -> tiles_y * s -> tiles_x * sizeof(int));
  
	REYNOLDS_VERBOSE("Resetting board: %d rows, %d cols", s -> tiles_y, s -> tiles_x);
	place_mines(s);
}