#include "app_input.h"

#include "common.h"
#include <core/window.h>
#include <core/input.h>
#include <OrthoCameraController.h>

#include "game_logic.h"
#include "board.h"
#include "display.h"
#include "file_handling.h"

#define CONFIG_NUM		7

					/* CONSTANTS */

const char* BOARD_CONFIG_PATH = "../testbed/res/extra/boards.json";

// Enumerations for button types and mouse actions
enum button_types 	{ PLAY_BUTTON = 0, EASY, INTERMEDIATE, HARD, RESTART_BUTTON };
enum mouse 			{ LEFT_CLICK = 0, RIGHT_CLICK = 1 };

// Predefined button bounds for UI elements
const int BUTTON_BOUNDS[][4] = {
	[PLAY_BUTTON] 		= {348, 620, 750, 860},
    [EASY] 				= {380, 586, 235, 345},
    [INTERMEDIATE] 		= {348, 620, 425, 540},
    [HARD] 				= {348, 620, 620, 735},
	[RESTART_BUTTON] 	= {380, 586, 616, 677} 
};

static void toBoardSpace(double* xpos, double* ypos) 
{
    float board_width  = (float)getBoardWidth();
    float board_height = (float)getBoardHeight();
    
    *xpos = *xpos * board_width / (float)getWindowWidth();
    *ypos = *ypos * board_height / (float)getWindowHeight();
}

static void applyZoom(double* xpos, double* ypos, float zoom_factor) 
{
    *xpos /= zoom_factor;
    *ypos /= zoom_factor;
}

static void calculateGridOffsets(double* grid_offset_x, double* grid_offset_y, float zoom_factor) 
{
    float board_width  = (float)getBoardWidth();
    float board_height = (float)getBoardHeight();
    float grid_width   = (float)getGridWidth();
    float grid_height  = (float)getGridHeight();

    float* camera_pos = getCameraPosition();
    float* position_offset = getConfigPosition();

    double grid_translation_x = (1.0f + position_offset[0] - camera_pos[0]) * board_width / 2.0f - (1.0f + position_offset[0]) * grid_width / 2.0f;
    double grid_translation_y = (1.0f + position_offset[1] + camera_pos[1]) * board_height / 2.0f;

    *grid_offset_x = ((board_width - grid_width) / 2.0 + grid_translation_x) / zoom_factor;
    *grid_offset_y = ((board_height - grid_height) / 2.0 + grid_translation_y) / zoom_factor;
}

static void calculateTileSize(double* tile_size_x, double* tile_size_y, float zoom_factor) 
{
    float grid_width  = (float)getGridWidth();
    float grid_height = (float)getGridHeight();
    float tiles_x     = (float)getBoardCols();
    float tiles_y     = (float)getBoardRows();
    float* size_offset = getConfigSize();
    
    *tile_size_x = (size_offset[0] / 2.0f) * grid_width / (zoom_factor * tiles_x);
    *tile_size_y = (size_offset[1] / 2.0f) * grid_height / (zoom_factor * tiles_y);
}

void getGridIndices(double xpos, double ypos, int* grid_coords) 
{
    REYNOLDS_DEBUG("xpos: %f, ypos: %f", xpos, ypos);

    float zoom_factor = getZoomLevel();
    
    toBoardSpace(&xpos, &ypos);
    applyZoom(&xpos, &ypos, 1.0f);

    double grid_offset_x, grid_offset_y;
    calculateGridOffsets(&grid_offset_x, &grid_offset_y, 1.0f);
    
    double tile_size_x, tile_size_y;
    calculateTileSize(&tile_size_x, &tile_size_y, 1.0f);

    // Adjust mouse position relative to grid
    double adjustedX = xpos - grid_offset_x;
    double adjustedY = ypos - grid_offset_y;

    // Check if the mouse is out of bounds
    float tiles_x = (float)getBoardCols();
    float tiles_y = (float)getBoardRows();
    if (adjustedX < 0 || adjustedX >= tile_size_x * tiles_x ||
        adjustedY < 0 || adjustedY >= tile_size_y * tiles_y) {
        grid_coords[0] = -1; // Out of bounds
        grid_coords[1] = -1;
        return;
    }

    // Calculate grid coordinates
    grid_coords[0] = (int)(adjustedX / tile_size_x);
    grid_coords[1] = (int)(adjustedY / tile_size_y);
}


void KeyInput()
{
	/* Function handles keyboard input for the game */
	
	if(isKeyPressed(GLFW_KEY_R) && game_state == PLAY)
	{		
		game_state = RESTART;

	} else if (isKeyPressed(GLFW_KEY_M) && (scene == OPTIONS || game_state == PLAY))
	{
		REYNOLDS_DEBUG("@instance: scene switched to menu");
		scene 		= MENU;
		game_state 	= NONE;			
		win 		= false;
		lose 		= false;
		init_state	= true;
	}
}

static bool click_bounds(double xpos, double ypos, const int* bounds)
{
	/*
		The input to this function is the current cursor position and the bounding rectangle.
		The function returns true if the cursor is within the bounds.
	*/
	
	int x_inf = bounds[0];
	int x_sup = bounds[1];
	int y_inf = bounds[2];
	int y_sup = bounds[3];
	
	return (xpos > x_inf && xpos < x_sup) && (ypos > y_inf && ypos < y_sup);
}

					/* MOUSE LOGIC FOR EACH SCENE */

static void mouse_during_menu(double xpos, double ypos)
{
	if(click_bounds(xpos, ypos, BUTTON_BOUNDS[PLAY_BUTTON]))
		scene = OPTIONS;
}

static void mouse_during_options(double xpos, double ypos)
{
	unsigned int config[CONFIG_NUM];
	
	if(click_bounds(xpos, ypos, BUTTON_BOUNDS[EASY]))
	{
		game_state = PLAY;
		scene = EASY_BOARD;
		
		ASSERT_LOG(load_board_config(BOARD_CONFIG_PATH, "easy", config), "Could not load board config.");
		
		init_state = true;
		init_board_state(config);
		
	} else if(click_bounds(xpos, ypos, BUTTON_BOUNDS[INTERMEDIATE]))
	{			
		game_state = PLAY;
		scene = INTER_BOARD;
		
		ASSERT_LOG(load_board_config(BOARD_CONFIG_PATH, "intermediate", config), "Could not load board config.");
		
		init_state = true;
		init_board_state(config);
							
	} else if(click_bounds(xpos, ypos, BUTTON_BOUNDS[HARD]))
	{					
		game_state = PLAY;
		scene = HARD_BOARD;
		
		ASSERT_LOG(load_board_config(BOARD_CONFIG_PATH, "hard", config), "Could not load board config.");
		
		init_state = true;
		init_board_state(config);
	}
}

static void mouse_during_play(double xpos, double ypos, bool right_click )
{
	int grid_coords[2];
	int gridX, gridY;
	
	getGridIndices(xpos, ypos, grid_coords);
	REYNOLDS_DEBUG("Zoom level: %f", 1/getZoomLevel());
	gridX = grid_coords[0], gridY = grid_coords[1];
	
	if(valid_tile(gridX, gridY))
	{
		int** reveal_state = getRevealState();
		VALIDATE_LOG(reveal_state, "Application trying to access non-existant board memory!");
		
		if(right_click)
		{
			// handle right click to flag tiles
			if (valid_flag(gridX, gridY))
				reveal_state[gridY][gridX] = TILE_FLAGGED;
			
			REYNOLDS_DEBUG("@input: tile right clicked at: (%d, %d)", gridX, gridY);
			
		} else {
			int** board = getBoardState();
			VALIDATE_LOG(board, "Application trying to access non-existant board memory!");
			
			// handle left click to all tiles		
			if(reveal_state[gridY][gridX] == TILE_UNFLAGGED)
				reveal_state[gridY][gridX] = TILE_CLOSED;
			
			reveal_tile(gridX, gridY);
			init_state = false;

			lose = (board[gridY][gridX] == BOMB && reveal_state[gridY][gridX] == TILE_CLEARED);
			
			REYNOLDS_DEBUG("@input: tile left clicked at: (%d, %d)", gridX, gridY);
		}
	} else REYNOLDS_DEBUG("@input: clicked out of bounds");	
}

static void mouse_during_end(double xpos, double ypos)
{
	if(click_bounds(xpos, ypos, BUTTON_BOUNDS[RESTART_BUTTON]))
	{
		game_state 	= RESTART;
		init_state 	= true;
		win 		= false;
		lose 		= false;
	}
} 
//

static void click_logic(int instance, bool mouse_click)
{
	/* Function handles mouse input for different scenes and states. */
	
	double xpos = getMouseX();
	double ypos = getMouseY();
		
	// instance may be any enum from game_instance struct (now defunct)
	if(instance & MENU)
		mouse_during_menu(xpos, ypos);
	else if(instance & OPTIONS)
		mouse_during_options(xpos, ypos);
	else if(instance & PLAY)
		mouse_during_play(xpos, ypos, mouse_click);
	else if(instance & GAME_OVER)
		mouse_during_end(xpos, ypos);	
}

bool handled = false;

void MouseInput() 
{
	/* Function handles mouse input for the game. */

	if(!handled)
	{	
		if (isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) 
		{			
			handled = true;
			click_logic(scene | game_state, LEFT_CLICK);
		} 
		else if (isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) 
		{		
			handled = true;
			click_logic(game_state, RIGHT_CLICK);
		}
	}
	
	if (!isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) 
		handled = false;
}