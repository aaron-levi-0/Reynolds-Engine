#include "display.h"

#include "common.h"
#include "game_logic.h"
#include "board.h"
#include "app_input.h"

#include <texture.h>
#include <Camera/OrthoCameraController.h>
#include <subTexture.h>

vec4 FULL_SAMPLE = {0.0f, 0.0f, 1.0f, 1.0f};

enum choose_scene { EASY, INTERMEDIATE, HARD };
#define DEFAULT	(INTERMEDIATE)

#define SPRITE_AMOUNT 12	

enum sprites { CLOSED_TILE = 0, ONE_TILE, TWO_TILE, THREE_TILE, FOUR_TILE, FIVE_TILE, 
				SIX_TILE, SEVEN_TILE, EIGHT_TILE, NINE_TILE, FLAG_TILE, BOMB_TILE};

uint16_t sprite_cell[][2] = {
	[CLOSED_TILE] 	= {0, 2},
	[ONE_TILE]		= {1, 2},
	[TWO_TILE]		= {2, 2},
	[FOUR_TILE]		= {0, 1},
	[THREE_TILE]	= {3, 2},
	[FIVE_TILE]		= {1, 1},
	[SIX_TILE]		= {2, 1},
	[SEVEN_TILE]	= {3, 1},
	[EIGHT_TILE]	= {0, 0},
	[NINE_TILE]		= {1, 0},
	[FLAG_TILE] 	= {2, 0},
	[BOMB_TILE]		= {3, 0}
}; //NOTE: sprite sheet sampling starts from bottom left TO-DO: fix order

vec4 sprite_uv[SPRITE_AMOUNT] = {0};

void init_display(DisplayContext* dc)
{
	subtex_init(&dc -> texture_register, MAX_TEXTURE_SLOTS);

	dc -> easy_board		= LoadTexture("../testbed/res/board_easy.png");
	dc -> inter_board		= LoadTexture("../testbed/res/board_classic.png");
	dc -> hard_board		= LoadTexture("../testbed/res/board_expert.png");
	dc -> menu_texture		= LoadTexture("../testbed/res/main_menu.png");
	dc -> options_texture	= LoadTexture("../testbed/res/options.png");
	dc -> board_pieces		= LoadTexture("../testbed/res/spritemap.png");
	dc -> button_restart	= LoadTexture("../testbed/res/button_restart.png");
	dc -> game_over			= LoadTexture("../testbed/res/game_over.png");
	dc -> win_overlay		= LoadTexture("../testbed/res/win_overlay.png");

	dc -> sprite_cell_size[0] = 46.0f; 	dc -> sprite_cell_size[1] = 46.0f;
	dc -> sprite_size[0] = 1.0f; 		dc -> sprite_size[1] = 1.0f;

	for (int id = CLOSED_TILE; id <= BOMB_TILE; id++) 
	{
		bool ok = subtex_register_cell(&dc -> texture_register,
									dc -> board_pieces,
									sprite_cell[id],
									dc -> sprite_cell_size,
									dc -> sprite_size);
		VALIDATE_LOG(ok, "Could not register sprite id %d", id);
		glm_vec4_copy(get_uv(&dc -> texture_register, dc -> board_pieces, sprite_cell[id]), sprite_uv[id]);
	}

	dc -> bg_colour[0] = 0.57f;
	dc -> bg_colour[1] = 0.51f;
	dc -> bg_colour[2] = 0.55f;
}

void init_scenes(DisplayContext* dc, GameState* s)
{
    s -> scenes[EASY]         = (SceneConfig){{-0.5f,-0.5f},{1.0f,1.0f}, dc -> easy_board}; //TO-DO: stop zoom-in making tex blurry
    s -> scenes[INTERMEDIATE] = (SceneConfig){{-1.0f,-1.0f},{2.0f,2.0f}, dc -> inter_board};
    s -> scenes[HARD]         = (SceneConfig){{-0.75f,-0.4f},{1.5f,0.8f}, dc -> hard_board};
}

void calc_norms(DisplayContext* dc, float* params[]) 
{
	float cols 			= (float)dc -> tiles_x;
	float rows 			= (float)dc -> tiles_y;
	float width 		= (float)dc -> width;
	float height 		= (float)dc -> height;
	float grid_width 	= (float)dc -> grid_width;
	float grid_height 	= (float)dc -> grid_height;
	
	float* size = dc -> board_size;

	// Grid and image parameters
	float tile_size_x = grid_width/cols - (float)LINE_THICKNESS;  				// tile size in pixels
	float tile_size_y = grid_height/rows - (float)LINE_THICKNESS;  				// tile size in pixels
	
	float grid_offset_x = (width - grid_width - (float)LINE_THICKNESS)/2.0f;	// grid_offset in pixels
	float grid_offset_y = (height - grid_height - (float)LINE_THICKNESS)/2.0f;	// grid_offset in pixels
	
	float grid_world_offset_x = size[0] * grid_offset_x/width;  				// Scaled grid size in worldspace
	float grid_world_offset_y = size[1] * grid_offset_y/height;  				// Scaled grid size in worldspace
	float tile_world_w = size[0] * tile_size_x/width; 							// Tile size in worldspace
	float tile_world_h = size[1] * tile_size_y/height; 							// Tile size in worldspace
	
	float grid_world_spacing_x = size[0]*(float)LINE_THICKNESS/width; 			// Tile spacing in worldspace
	float grid_world_spacing_y = size[1]*(float)LINE_THICKNESS/height; 			// Tile spacing in worldspace
	
	**params++ 	= grid_world_offset_x;
	**params++ 	= grid_world_offset_y;
	**params++ 	= tile_world_w;
	**params++ 	= tile_world_h;
	**params++ 	= grid_world_spacing_x;
	**params 	= grid_world_spacing_y; //UNCLEAR: do i need all this here?
}

static void draw_piece(struct Renderer* renderer, DisplayContext* dc, GameState* s,  int x, int y, vec2 pos, vec2 size)
{
	Tile* board = s -> board;
	VALIDATE_LOG(renderer, "Attempting to draw on an uninitialised renderer!");
	VALIDATE_LOG(s -> board, "Application trying to access non-existant board memory!");
	VALIDATE_LOG(valid_tile(dc, x, y), "Invalid board coordinates: (%d, %d)", x, y);

	uint8_t reveal_state = board[IDX(x, y)].state;
	int8_t board_value = board[IDX(x, y)].value;

	if(reveal_state == IS_TILE_CLOSED || reveal_state == IS_TILE_UNFLAGGED)
	{	
		DrawQuad(renderer, pos, size, dc -> board_pieces, sprite_uv[CLOSED_TILE]);
	}
	
	else if(reveal_state == IS_TILE_FLAGGED)
	{
		DrawQuad(renderer, pos, size, dc -> board_pieces, sprite_uv[CLOSED_TILE]);
		DrawQuad(renderer, pos, size, dc -> board_pieces, sprite_uv[FLAG_TILE]);
	} 
	else if(board_value != BOMB)
	{
		if(board[IDX(x, y)].value == EMPTY) return;
		DrawQuad(renderer, pos, size, dc -> board_pieces, sprite_uv[board[IDX(x, y)].value]);
	}
	else
		DrawQuad(renderer, pos, size, dc -> board_pieces, sprite_uv[BOMB_TILE]);
}

//This is an overlay that tells the user that the game is now complete and gives them the option to restart
static void draw_overlay(struct Renderer* renderer, DisplayContext* dc, GameState* s) 
{	
	// Dimensions for the white box
	vec2 boxPos = {-10.0f/23.0f, -0.5f};  // Centered on screen
	vec2 boxSize = {20.0f/23.0f, 14.0f/23.0f};   // Size of the box
	
	if(s -> game_over_overlay) // if s -> game_over
		DrawQuad(renderer, boxPos, boxSize, dc -> game_over, FULL_SAMPLE);
	else
		DrawQuad(renderer, boxPos, boxSize, dc -> win_overlay, FULL_SAMPLE);
	
	// Dimensions for the button
	vec2 buttonPos = {-5.0f/23.0f, -0.4f};
	vec2 buttonSize = {10.0f/23.0f, 3.0f/23.0f};
	DrawQuad(renderer, buttonPos, buttonSize, dc -> button_restart, FULL_SAMPLE);
}

static void draw_board(struct Renderer* renderer, DisplayContext* dc, GameState* s)
{
	uint8_t rows = dc -> tiles_y;
	uint8_t cols = dc -> tiles_x;
	
	VALIDATE_LOG(renderer, "Attempting to draw on an uninitialised renderer!");
	
	float norm_grid_offset_x, 	norm_grid_offset_y;
	float norm_tile_size_x, 	norm_tile_size_y;						  
	float norm_spacing_x, 		norm_spacing_y;
	
	calc_norms(dc, (float* []){&norm_grid_offset_x, &norm_grid_offset_y, &norm_tile_size_x,
			&norm_tile_size_y, &norm_spacing_x, &norm_spacing_y}); //tiling function
	
	float xpos, ypos;
	
	vec2 piece_pos, piece_size = {norm_tile_size_x, norm_tile_size_y};

	// Loop to render tiles
	for (uint8_t i = 1; i < rows + 1; i++) 
	{
		for (uint8_t j = 0; j < cols; j++) 
		{			
			// Compute normalized tile position
			xpos = dc -> board_pos[0] + norm_grid_offset_x + norm_spacing_x + j * (norm_tile_size_x + norm_spacing_x);
			ypos = -dc -> board_pos[1] - norm_grid_offset_y - i * (norm_tile_size_y + norm_spacing_y);

			// Normalized size for the tile
			piece_pos[0] = xpos;
			piece_pos[1] = ypos;

			draw_piece(renderer, dc, s, j, i - 1, piece_pos, piece_size);
		}
	}
}

static void board_controller(struct Renderer* renderer, GameState* s, DisplayContext* dc, int choose_scene) 
{
   	dc -> board_pos[0] = s -> scenes[choose_scene].pos[0];
	dc -> board_pos[1] = s -> scenes[choose_scene].pos[1];

	dc -> board_size[0] = s -> scenes[choose_scene].size[0];
	dc -> board_size[1] = s -> scenes[choose_scene].size[1];
	dc -> board_type = s -> scenes[choose_scene].texture;

	DrawColour(renderer, (vec2){-1.0f, -1.0f}, (vec2){2.0f, 2.0f}, (vec4){0.57f, 0.51f, 0.55f, 1.0f});
    DrawQuad(renderer, dc -> board_pos, dc -> board_size, dc -> board_type, FULL_SAMPLE);
	draw_board(renderer, dc, s);
}

void ui_scene_update(DisplayContext* dc, GameState* s)
{
	if (s -> scene == MENU || s -> scene == OPTIONS)
		resetOrthoCamera();
}

void scene_update(DisplayContext* dc, GameState* s)
{	
    // Handle game-state transitions
    if (s -> game_state == RESTART)
	{
		REYNOLDS_DEBUG("@instance: reset implemented");
		reset_board(dc, s);

		s -> win		= false;
		s -> lose		= false;
		s -> game_state	= PLAY;
    }
}

void ui_scene_render(struct Renderer* renderer, DisplayContext* dc, GameState* s)
{
	ASSERT_FATAL(renderer, "Attempting to draw on an uninitialised renderer!");

	if(s -> scene == MENU || s -> scene == OPTIONS)
	{
		dc -> board_pos[0]  = s -> scenes[DEFAULT].pos[0];
		dc -> board_pos[1]  = s -> scenes[DEFAULT].pos[1];
		dc -> board_size[0] = s -> scenes[DEFAULT].size[0];
		dc -> board_size[1] = s -> scenes[DEFAULT].size[1];
	}
 
	switch (s -> scene)
	{
		case MENU:
			DrawQuad(renderer, dc -> board_pos, dc -> board_size, dc -> menu_texture, FULL_SAMPLE);
			break;

		case OPTIONS:
			DrawQuad(renderer, dc -> board_pos, dc -> board_size, dc -> options_texture, FULL_SAMPLE);
			break;
	}

	if (s -> game_state == GAME_OVER)
        draw_overlay(renderer, dc, s);
}

void scene_render(struct Renderer* renderer, DisplayContext* dc, GameState* s)
{
    ASSERT_FATAL(renderer, "Attempting to draw on an uninitialised renderer!");
 
    switch (s -> scene)
    {
        case EASY_BOARD:
            SetClearColour(dc -> bg_colour);
            board_controller(renderer, s, dc, EASY);
            break;

        case INTER_BOARD:
            SetClearColour(dc -> bg_colour);
            board_controller(renderer, s, dc, INTERMEDIATE);
            break;

        case HARD_BOARD:
            SetClearColour(dc -> bg_colour);
            board_controller(renderer, s, dc, HARD);
            break;
    }

	if (s -> game_state == GAME_OVER)
        draw_overlay(renderer, dc, s);
}

void unload_textures(DisplayContext* dc)
{
	subtex_shutdown(&dc -> texture_register);
}