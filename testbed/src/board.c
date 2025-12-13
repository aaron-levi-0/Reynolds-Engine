#include "board.h"

#include "common.h"
#include <core/window.h>

static BOARD board = {.board = NULL, .reveal_state = NULL};

int** getBoardState()	{ return board.board; }
int** getRevealState() 	{ return board.reveal_state; }

uint16_t getBoardWidth() { return board.width; }
uint16_t getBoardHeight() { return board.height; }
uint16_t getGridWidth() { return board.grid_width; }
uint16_t getGridHeight() { return board.grid_height; }
uint8_t getBoardRows() { return board.tiles_y; }
uint8_t getBoardCols() { return board.tiles_x; }

int valid_tile(int x, int y)
{
	/* Function to check bounds */
    return ((x >= 0 && x < (int) board.tiles_x) && (y >= 0 && y < (int) board.tiles_y));
}

void place_mines() 
{
	int x, y;
	
    srand(time(NULL));
    for (uint16_t i = 0; i < board.num_bombs; i++) 
	{
         do {
			 
            x = rand() % board.tiles_x;	
            y = rand() % board.tiles_y;
			
        } while (board.board[y][x] == BOMB);	//y tiles are the height which are the rows
		
        board.board[y][x] = BOMB;
		REYNOLDS_VERBOSE("Placing mine at (%d, %d)", x, y);
		
        // Increment numbers around the bomb
        for (int dx = -1; dx <= 1; dx++) 
		{
            for (int dy = -1; dy <= 1; dy++) 
			{
                int nx = x + dx, ny = y + dy;
                if (valid_tile(nx, ny) && board.board[ny][nx] != BOMB)
					board.board[ny][nx]++;
            }
        }
    }
}

void delete_board() 
{
	ASSERT_LOG(board.board, "Attempting to free board memory which doesn't exist!");
	ASSERT_LOG(board.reveal_state, "Attempting to free board memory which doesn't exist!");

	free(board.board[0]);
	free(board.reveal_state[0]);
	
	free(board.board);
	free(board.reveal_state);
	
	board.board 		= NULL;
	board.reveal_state 	= NULL;
}

void init_board_state(uint32_t* configs)
{
	if(board.board || board.reveal_state)
		delete_board();
	
	board.width = *configs++, 		board.height = *configs++;
	board.grid_width = *configs++,	board.grid_height = *configs++;
	board.tiles_x = *configs++, 	board.tiles_y = *configs++;			// need to check if len of configs[] is exactly 7*int?
	board.num_bombs = *configs;
		
	ASSERT_LOG(board.width > 0, 		"board size is invalid!");
	ASSERT_LOG(board.height > 0, 		"board size is invalid!");
	ASSERT_LOG(board.grid_width > 0,	"grid size is invalid!");
	ASSERT_LOG(board.grid_height > 0,	"grid size is invalid!");
	ASSERT_LOG(board.tiles_x > 0, 		"number of tiles is invalid!");
	ASSERT_LOG(board.tiles_y > 0, 		"number of tiles is invalid!");
	ASSERT_LOG(board.num_bombs > 0, 	"number of bombs is invalid!");
	
	uint8_t row = board.tiles_y;
	uint8_t col = board.tiles_x;
	uint16_t total_cells = row * col;
	
	//create row pointers
	board.board 		= malloc(row * sizeof(int*));
	board.reveal_state 	= malloc(row * sizeof(int*));
	
	ASSERT_LOG(board.board, "Failed to allocate memory for board!\n");
	ASSERT_LOG(board.reveal_state, "Failed to allocate memory for board!\n");
	
	//set the contiguos board memory
	board.board[0] 			= calloc(total_cells, sizeof(int));
	board.reveal_state[0] 	= calloc(total_cells, sizeof(int));
	
	ASSERT_LOG(board.board[0], "Failed to allocate memory for board!\n");
	ASSERT_LOG(board.reveal_state[0], "Failed to allocate memory for board!\n");
	
	for (uint8_t i = 0; i < row; i++)
	{	
		//set the row pointers
		board.board[i] 			= board.board[0] + i * col;
		board.reveal_state[i] 	= board.reveal_state[0] + i * col;
	}
	
	place_mines();
}

//TODO: move NDC conversion into engine
void calc_norms(vec2 size, float* params[])
{
	float cols 			= (float)board.tiles_x;
	float rows 			= (float)board.tiles_y;
	float width 		= (float)board.width;
	float height 		= (float)board.height;
	float grid_width 	= (float)board.grid_width;
	float grid_height 	= (float)board.grid_height;
	
	// Grid and image parameters
	float tile_size_x = grid_width/cols - (float)LINE_THICKNESS;  				// tile size in pixels
	float tile_size_y = grid_height/rows - (float)LINE_THICKNESS;  				// tile size in pixels
	
	float grid_offset_x = (width - grid_width - (float)LINE_THICKNESS)/2.0f;	// grid_offset in pixels
	float grid_offset_y = (height - grid_height - (float)LINE_THICKNESS)/2.0f;	// grid_offset in pixels
	
	float norm_grid_offset_x = size[0] * grid_offset_x/width;  					// Scaled grid size in NDC
	float norm_grid_offset_y = size[1] * grid_offset_y/height;  				// Scaled grid size in NDC
	float norm_tile_size_x = size[0] * tile_size_x/width; 						// Tile size in NDC
	float norm_tile_size_y = size[1] * tile_size_y/height; 						// Tile size in NDC
	
	float norm_spacing_x = size[0]*(float)LINE_THICKNESS/width; 				// Tile spacing in NDC
	float norm_spacing_y = size[1]*(float)LINE_THICKNESS/height; 				// Tile spacing in NDC
	
	**params++ 	= norm_grid_offset_x;
	**params++ 	= norm_grid_offset_y;
	**params++ 	= norm_tile_size_x;
	**params++ 	= norm_tile_size_y;
	**params++ 	= norm_spacing_x;
	**params 	= norm_spacing_y;
}

void reset_board() 
{
	// Clearing board state of bombs and numbers, closing all revealed tiles
	memset(board.board[0], EMPTY, board.tiles_y * board.tiles_x * sizeof(int));
	memset(board.reveal_state[0], TILE_CLOSED, board.tiles_y * board.tiles_x * sizeof(int));
  
	REYNOLDS_VERBOSE("Resetting board: %d rows, %d cols", board.tiles_y, board.tiles_x);
	place_mines();
}