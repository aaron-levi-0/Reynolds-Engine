#include "game_logic.h"

#include "common.h"
#include "board.h"

bool win = false, lose = false;

void reveal_tile(int x, int y)
{
	/* 	If a tile is closed, the function will mark it to be revealed. If the tile is a clear 
		tile, a flood-fill algorithm is then implemented to reveal all neighbouring clear tiles
		as well.
	*/
	
	int** board 		= getBoardState();
	int** reveal_state 	= getRevealState();
	
	VALIDATE_LOG(board, "Application trying to access non-existant board memory!");
	VALIDATE_LOG(reveal_state, "Application trying to access non-existant board memory!");

    if (!valid_tile(x, y) || reveal_state[y][x] != TILE_CLOSED) return;

    reveal_state[y][x] = TILE_CLEARED;

    if (board[y][x] != 0) return; // Stop if it's a numbered tile

    // Recursively reveal neighbors (flood-fill algorithm)
    for (int8_t dx = -1; dx <= 1; dx++) 
	{
        for (int8_t dy = -1; dy <= 1; dy++) 
		{
			// Avoid self-loop
            if (dx != 0 || dy != 0) 
                reveal_tile(x + dx, y + dy);
        }
    }
}

bool valid_flag(int x, int y)
{	
	/* 	Checks to see if the tile chosen is a valid tile to flag. If the tile
		is already flagged, it is marked as unflagged.
	*/
	
	int** reveal_state 	= getRevealState();
	ASSERT_LOG(reveal_state, "Application trying to access non-existant board memory!");
	
	if (!valid_tile(x, y)) return false;
	
	if(reveal_state[y][x] == TILE_CLOSED || reveal_state[y][x] == TILE_UNFLAGGED)
		return true;
	
	if(reveal_state[y][x] == TILE_FLAGGED)
		reveal_state[y][x] = TILE_UNFLAGGED;
	
	return false;
}

bool check_win() 
{
	/* 	Checks the current board state. Returns true if all bombs have 
		been flagged and all non-bomb tiles remain unflagged.
	*/
	
	int** board 		= getBoardState();
	int** reveal_state 	= getRevealState();
	
	ASSERT_LOG(board, "Application trying to access non-existant board memory!");
	ASSERT_LOG(reveal_state, "Application trying to access non-existant board memory!");
	
	uint8_t row = getBoardRows();
	uint8_t col = getBoardCols();
	
    for (uint8_t i = 0; i < row; i++) {
        for (uint8_t j = 0; j < col; j++) {
            if (board[i][j] == BOMB && reveal_state[i][j] != TILE_FLAGGED) {
                return false; // Bomb not flagged
            }
            if (board[i][j] != BOMB && reveal_state[i][j] == TILE_FLAGGED) {
                return false; // Non-bomb cell incorrectly flagged
            }
        }
    }
    return true;
}