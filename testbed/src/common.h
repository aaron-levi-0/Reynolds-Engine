#ifndef COMMON_H
#define COMMON_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <cglm/cglm.h>
#include <core/logging.h>

#define SCREEN_WIDTH	970
#define SCREEN_HEIGHT	970

#define NOT_USED		0
#define BIT(x) 			(1 << x)


#define ALL_BOARDS		448		//this is 0b11100000 or 448 as an integer

enum tile_state {IS_TILE_CLOSED, IS_TILE_CLEARED, IS_TILE_FLAGGED, IS_TILE_UNFLAGGED};
enum game_state {NONE = BIT(0), PLAY = BIT(1), GAME_OVER = BIT(2), RESTART = BIT(3)};
enum scene 		{MENU = BIT(4), OPTIONS = BIT(5), EASY_BOARD = BIT(6), INTER_BOARD = BIT(7), HARD_BOARD = BIT(8)};
	
/* typedef struct{
	enum {TILE_CLOSED, TILE_CLEARED, TILE_FLAGGED, TILE_UNFLAGGED} tile_state;
	enum {NONE = BIT(0), PLAY = BIT(1), GAME_OVER = BIT(2), RESTART = BIT(3)} game_state;
	enum {MENU = BIT(4), OPTIONS = BIT(5), EASY_BOARD = BIT(6), INTER_BOARD = BIT(7), HARD_BOARD = BIT(8)} scene;
} game_instance; */

// NOTE: difference between closed and unflagged, it goes from flagged -> unflagged -> closed. This essentially 
// skips a draw call and helps to stop immediately revealing the tile after unflagging it

#endif

//assert.h