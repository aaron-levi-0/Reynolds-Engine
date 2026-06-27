#include "app_input.h"

#include <window.h>
#include <Camera/camera.h>

#include "common.h"
#include "game_logic.h"
#include "board.h"
#include "display.h"
#include "file_handling.h"

#define CONFIG_NUM		7

					/* CONSTANTS */

const char* BOARD_CONFIG_PATH = "../testbed/res/extra/boards.json";

// Enumerations for button types and mouse actions
enum button_types 	{ PLAY_BUTTON = 0, EASY_BUTTON, INTER_BUTTON, HARD_BUTTON, RESTART_BUTTON };
enum mouse 			{ LEFT_CLICK = 0, RIGHT_CLICK = 1 };

// Predefined button bounds for UI elements
const int BUTTON_BOUNDS[][4] = {
	[PLAY_BUTTON] 		= {348, 620, 750, 860},
    [EASY_BUTTON] 		= {380, 586, 235, 345},
    [INTER_BUTTON] 		= {348, 620, 425, 540},
    [HARD_BUTTON] 		= {348, 620, 620, 735},
	[RESTART_BUTTON] 	= {380, 586, 616, 677} 
};

static void screen_to_world(double sx, double sy, float* out_world_xy)
{
    // 1) pixels -> NDC
    float w = (float)getWindowWidth();
    float h = (float)getWindowHeight();

    float ndc_x = (float)( (2.0 * sx) / w - 1.0 );
    float ndc_y = (float)( 1.0 - (2.0 * sy) / h ); // y flipped (top-left -> NDC)

    // 2) NDC -> world via inverse(PV)
    mat4 invPV;
    glm_mat4_inv(getPVMat(), invPV);

    vec4 clip = { ndc_x, ndc_y, 0.0f, 1.0f };
    vec4 world;
    glm_mat4_mulv(invPV, clip, world);

    // Ortho camera: w should be 1, but divide anyway for safety
    float iw = (world[3] != 0.0f) ? (1.0f / world[3]) : 1.0f;

    out_world_xy[0] = world[0] * iw;
    out_world_xy[1] = world[1] * iw;
}

void getGridIndices(DisplayContext* dc, double mouse_px_x, double mouse_px_y, int* grid_coords)
{
    // Convert mouse to the same world space that DrawQuad uses
    float world_xy[2];
    screen_to_world(mouse_px_x, mouse_px_y, world_xy);

    // Pull the same config + norms used by draw_board() in display.c

    float norm_grid_offset_x, norm_grid_offset_y;
    float norm_tile_size_x,  norm_tile_size_y;
    float norm_spacing_x,    norm_spacing_y;

    calc_norms(dc, (float*[]){
        &norm_grid_offset_x, &norm_grid_offset_y,
        &norm_tile_size_x,   &norm_tile_size_y,
        &norm_spacing_x,     &norm_spacing_y
    });

	const float* pos 	= dc -> board_pos;
    const int cols	 	= dc -> tiles_x;
    const int rows 		= dc -> tiles_y;

    const float step_x = norm_tile_size_x + norm_spacing_x;
    const float step_y = norm_tile_size_y + norm_spacing_y;

    // This matches your rendering placement:
    // x = pos.x + norm_grid_offset_x + norm_spacing_x + j*step_x
    // y = -pos.y - norm_grid_offset_y - i*step_y (with i starting at 1)
    const float origin_x = pos[0] + norm_grid_offset_x + norm_spacing_x;
    const float origin_y = -pos[1] - norm_grid_offset_y;

    const float rel_x = world_xy[0] - origin_x;
    const float rel_y = origin_y - world_xy[1]; // because y decreases as row increases

    if (rel_x < 0.0f || rel_y < 0.0f) { grid_coords[0] = -1; grid_coords[1] = -1; return; }

    const int gx = (int)floorf(rel_x / step_x);
    const int gy = (int)floorf(rel_y / step_y);

    if (gx < 0 || gx >= cols || gy < 0 || gy >= rows) { grid_coords[0] = -1; grid_coords[1] = -1; return; }

    // Optional: reject clicks on grid-line spacing (so lines don’t “count” as tiles)
    const float in_cell_x = rel_x - (float)gx * step_x;
    const float in_cell_y = rel_y - (float)gy * step_y;
    if (in_cell_x > norm_tile_size_x || in_cell_y > norm_tile_size_y) {
        grid_coords[0] = -1; grid_coords[1] = -1; return;
    }

    grid_coords[0] = gx;
    grid_coords[1] = gy;
}


void KeyInput(GameState* s)
{
	/* Function handles keyboard input for the game */
	
	if(isKeyPressed(GLFW_KEY_R) && s -> game_state == PLAY)
	{		
		s -> game_state = RESTART;

	} else if (isKeyPressed(GLFW_KEY_M) && (s -> scene == OPTIONS || s -> game_state == PLAY))
	{
		REYNOLDS_DEBUG("@instance: scene switched to menu");
		s -> scene 		= MENU;
		s -> game_state = NONE;			
		s -> win 		= false;
		s -> lose 		= false;
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

static void mouse_during_menu(GameState* s, double xpos, double ypos)
{
	if(click_bounds(xpos, ypos, BUTTON_BOUNDS[PLAY_BUTTON]))
		s -> scene = OPTIONS;
}

static void mouse_during_options(DisplayContext* dc, GameState* s, double xpos, double ypos)
{
	unsigned int config[CONFIG_NUM];
	
	if(click_bounds(xpos, ypos, BUTTON_BOUNDS[EASY_BUTTON]))
	{
		s -> game_state = PLAY;
		s -> scene 		= EASY_BOARD;
		
		ASSERT_FATAL(load_board_config(BOARD_CONFIG_PATH, "easy", config), "Could not load board config.");
		
		init_board_state(dc, s, config);
		
	} else if(click_bounds(xpos, ypos, BUTTON_BOUNDS[INTER_BUTTON]))
	{			
		s -> game_state = PLAY;
		s -> scene 		= INTER_BOARD;
		
		ASSERT_FATAL(load_board_config(BOARD_CONFIG_PATH, "intermediate", config), "Could not load board config.");
		
		init_board_state(dc, s, config);
							
	} else if(click_bounds(xpos, ypos, BUTTON_BOUNDS[HARD_BUTTON]))
	{					
		s -> game_state = PLAY;
		s -> scene 		= HARD_BOARD;
		
		ASSERT_FATAL(load_board_config(BOARD_CONFIG_PATH, "hard", config), "Could not load board config.");
		
		init_board_state(dc, s, config);
	}
}

uint32_t ms_index(DisplayContext* dc, int x, int y)
{
	return (uint32_t)(y * dc -> tiles_x + x);
}

static void mouse_during_play(DisplayContext* dc, GameState* s, double xpos, double ypos, bool right_click)
{
	int grid_coords[2];
	int16_t gridX, gridY;
	
	getGridIndices(dc, xpos, ypos, grid_coords);
	gridX = grid_coords[0], gridY = grid_coords[1];
	
	if(valid_tile(dc, gridX, gridY))
	{
		Tile* board = s -> board; 
		VALIDATE_LOG(board, "Application trying to access non-existant board memory!");

		uint8_t* reveal_state = &board[IDX(gridX, gridY)].state;
		
		if(right_click)
		{
			// handle right click to flag tiles
			if (valid_flag(dc, s, gridX, gridY))
				*reveal_state = IS_TILE_FLAGGED;
			
			REYNOLDS_DEBUG("@input: tile right clicked at: (%d, %d)", gridX, gridY);
			
		} else {
			int8_t board_value = board[IDX(gridX, gridY)].value;
			
			// handle left click to all tiles		
			if(*reveal_state == IS_TILE_UNFLAGGED)
				*reveal_state = IS_TILE_CLOSED;
			
			reveal_tile(dc, s, gridX, gridY);

			s -> lose = (board_value == BOMB && *reveal_state == IS_TILE_CLEARED);
			
			REYNOLDS_DEBUG("@input: tile left clicked at: (%d, %d)", gridX, gridY);
		}
	} else REYNOLDS_DEBUG("@input: clicked out of bounds");	
}

static void mouse_during_end(GameState* s, double xpos, double ypos)
{
	if(click_bounds(xpos, ypos, BUTTON_BOUNDS[RESTART_BUTTON]))
	{
		s -> game_state = RESTART;
		s -> win 		= false;
		s -> lose 		= false;
	}
} 
//

static void click_logic(DisplayContext* dc, GameState* s, int instance, bool mouse_click)
{
	/* Function handles mouse input for different scenes and states. */
	
	double xpos = getMouseX();
	double ypos = getMouseY();
		
	// instance may be any enum from game_instance struct (now defunct)
	if(instance & MENU)
		mouse_during_menu(s, xpos, ypos);
	else if(instance & OPTIONS)
		mouse_during_options(dc, s, xpos, ypos);
	else if(instance & PLAY)
		mouse_during_play(dc, s, xpos, ypos, mouse_click);
	else if(instance & GAME_OVER)
		mouse_during_end(s, xpos, ypos);	
}

bool handled = false;

void MouseInput(DisplayContext* dc, GameState* s) 
{
	/* Function handles mouse input for the game. */

	if(!handled)
	{	
		if (isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) 
		{			
			handled = true;
			click_logic(dc, s, s -> scene | s -> game_state, LEFT_CLICK);
		} 
		else if (isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) 
		{		
			handled = true;
			click_logic(dc, s, s -> game_state, RIGHT_CLICK);
		}
	}
	
	if (!isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) 
		handled = false;
}