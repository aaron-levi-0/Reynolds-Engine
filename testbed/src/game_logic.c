#include "game_logic.h"

#include "common.h"
#include "board.h"
#include "app_input.h"

void reveal_tile(DisplayContext* dc, GameState* s, int x, int y)
{
	/* 	If a tile is closed, the function will mark it to be revealed. If the tile is a clear 
		tile, a flood-fill algorithm is then implemented to reveal all neighbouring clear tiles
		as well.
	*/

    Tile* board = s -> board;
    uint8_t reveal_state = board[IDX(x, y)].state;
    int8_t board_value = board[IDX(x, y)].value;

	
	VALIDATE_LOG(s -> board, "Application trying to access non-existant board memory!");

    if (!valid_tile(dc, x, y) || reveal_state != IS_TILE_CLOSED) return;
    
    reveal_state = IS_TILE_CLEARED;

    if (board_value != 0) return; // Stop if it's a numbered tile

    // Recursively reveal neighbors (flood-fill algorithm)
    for (int8_t dx = -1; dx <= 1; dx++) 
	{
        for (int8_t dy = -1; dy <= 1; dy++) 
		{
			// Avoid self-loop
            if (dx != 0 || dy != 0) 
                reveal_tile(dc, s, x + dx, y + dy);
        }
    }
}

bool valid_flag(DisplayContext* dc, GameState* s, int x, int y)
{	
	/* 	Checks to see if the tile chosen is a valid tile to flag. If the tile
		is already flagged, it is marked as unflagged.
	*/
	
	ASSERT_FATAL(s -> board, "Application trying to access non-existant board memory!");
	
    Tile* board = s -> board;
    uint8_t* reveal_state = &board[IDX(x, y)].state;

	if (!valid_tile(dc, x, y)) return false;
	
	if(*reveal_state == IS_TILE_CLOSED || *reveal_state == IS_TILE_UNFLAGGED)
		return true;
	
	if(*reveal_state == IS_TILE_FLAGGED)
		*reveal_state = IS_TILE_UNFLAGGED;
	
	return false;
}
bool check_win(DisplayContext* dc, GameState* s)
{
	/* 	Checks the current board state. Returns true if all bombs have 
		been flagged and all non-bomb tiles remain unflagged.
	*/

    ASSERT_FATAL(s -> board, "Application trying to access non-existant board memory!");

    Tile* board = s -> board;

    const uint8_t rows = dc -> tiles_y;
    const uint8_t cols = dc -> tiles_x;

    for (uint8_t y = 0; y < rows; y++) {
        for (uint8_t x = 0; x < cols; x++) {
            if (board[IDX(x, y)].value != BOMB && board[IDX(x, y)].state != IS_TILE_CLEARED) {
                return false; // some safe tile still hidden
            }
        }
    }
    return true;
}