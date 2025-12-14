#include "game_logic.h"

#include "common.h"
#include "board.h"

void reveal_tile(GameState* s, int x, int y)
{
	/* 	If a tile is closed, the function will mark it to be revealed. If the tile is a clear 
		tile, a flood-fill algorithm is then implemented to reveal all neighbouring clear tiles
		as well.
	*/
	
	VALIDATE_LOG(s -> board, "Application trying to access non-existant board memory!");
	VALIDATE_LOG(s -> reveal_state, "Application trying to access non-existant board memory!");

    if (!valid_tile(s, x, y) || s -> reveal_state[y][x] != TILE_CLOSED) return;

    s -> reveal_state[y][x] = TILE_CLEARED;

    if (s -> board[y][x] != 0) return; // Stop if it's a numbered tile

    // Recursively reveal neighbors (flood-fill algorithm)
    for (int8_t dx = -1; dx <= 1; dx++) 
	{
        for (int8_t dy = -1; dy <= 1; dy++) 
		{
			// Avoid self-loop
            if (dx != 0 || dy != 0) 
                reveal_tile(s, x + dx, y + dy);
        }
    }
}

bool valid_flag(GameState* s, int x, int y)
{	
	/* 	Checks to see if the tile chosen is a valid tile to flag. If the tile
		is already flagged, it is marked as unflagged.
	*/
	
	ASSERT_LOG(s -> reveal_state, "Application trying to access non-existant board memory!");
	
	if (!valid_tile(s, x, y)) return false;
	
	if(s -> reveal_state[y][x] == TILE_CLOSED || s -> reveal_state[y][x] == TILE_UNFLAGGED)
		return true;
	
	if(s -> reveal_state[y][x] == TILE_FLAGGED)
		s -> reveal_state[y][x] = TILE_UNFLAGGED;
	
	return false;
}
bool check_win(GameState* s)
{
	/* 	Checks the current board state. Returns true if all bombs have 
		been flagged and all non-bomb tiles remain unflagged.
	*/

    int** board        = s->board;
    int** reveal_state = s->reveal_state;

    ASSERT_LOG(board, "Application trying to access non-existant board memory!");
    ASSERT_LOG(reveal_state, "Application trying to access non-existant board memory!");

    const uint8_t rows = s->tiles_y;
    const uint8_t cols = s->tiles_x;

    for (uint8_t y = 0; y < rows; y++) {
        for (uint8_t x = 0; x < cols; x++) {
            if (board[y][x] != BOMB && reveal_state[y][x] != TILE_CLEARED) {
                return false; // some safe tile still hidden
            }
        }
    }
    return true;
}