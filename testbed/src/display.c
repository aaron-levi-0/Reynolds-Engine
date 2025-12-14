#include "display.h"

#include "common.h"
#include "game_logic.h"
#include "board.h"

#include <renderer/texture.h>
#include <OrthoCameraController.h>

const float MAP_OFFSET	= 46.0f; 	//NOTE: texture map subtexture offset (in pixels)

vec4 FULL_SAMPLE = {0.0f, 0.0f, 1.0f, 1.0f};

uint32_t menu_texture, options_texture;
uint32_t easy_board, inter_board, hard_board;
uint32_t board_pieces, button_restart, game_over, win_overlay;

typedef struct { vec2 pos, size; uint32_t texture; } SceneConfig;
SceneConfig scenes[3];

enum choose_scene { EASY, INTERMEDIATE, HARD };
#define DEFAULT	(INTERMEDIATE)

void init_textures()
{
	easy_board		= load_texture("../testbed/res/board_easy.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	inter_board		= load_texture("../testbed/res/board_classic.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	hard_board		= load_texture("../testbed/res/board_expert.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	menu_texture	= load_texture("../testbed/res/main_menu.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	options_texture	= load_texture("../testbed/res/options.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	board_pieces	= load_texture("../testbed/res/spritemap.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	button_restart	= load_texture("../testbed/res/button_restart.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	game_over		= load_texture("../testbed/res/game_over.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	win_overlay		= load_texture("../testbed/res/win_overlay.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	 
	setCellSize(MAP_OFFSET, MAP_OFFSET);
}

void init_scenes()
{
    scenes[EASY]         = (SceneConfig){{-0.5f,-0.5f},{1.0f,1.0f}, easy_board}; //TO-DO: stop zoom-in making tex blurry
    scenes[INTERMEDIATE] = (SceneConfig){{-1.0f,-1.0f},{2.0f,2.0f}, inter_board};
    scenes[HARD]         = (SceneConfig){{-0.75f,-0.4f},{1.5f,0.8f}, hard_board};
}

enum sprites { ONE_TILE = 1, TWO_TILE, THREE_TILE, FOUR_TILE, FIVE_TILE, 
				SIX_TILE, SEVEN_TILE, EIGHT_TILE, NINE_TILE, FLAG_TILE, BOMB_TILE};

vec2 sprite_size	= {1, 1};

vec2 sprite[] = {
	[TILE_CLOSED] 	= {0, 2},
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
}; //sprite sheet sampling starts from bottom left

void calc_norms(GameState* s, float* params[]) //TO-DO: move NDC conversion into engine
{
	float cols 			= (float)s -> tiles_x;
	float rows 			= (float)s -> tiles_y;
	float width 		= (float)s -> width;
	float height 		= (float)s -> height;
	float grid_width 	= (float)s -> grid_width;
	float grid_height 	= (float)s -> grid_height;
	
	float* size = s -> config_size;

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
	**params 	= grid_world_spacing_y; //TO-DO: do i need all this here?
}

static void draw_piece(GameState* s, struct Renderer* renderer, uint32_t texture, int x, int y, vec2 p_pos, vec2 p_size)
{
	VALIDATE_LOG(renderer, "Attempting to draw on an uninitialised renderer!");
	VALIDATE_LOG(s -> board, "Application trying to access non-existant board memory!");
	VALIDATE_LOG(s -> reveal_state, "Application is trying to access non-existant board memory!");
	VALIDATE_LOG(valid_tile(s, x, y), "Invalid board coordinates: (%d, %d)", x, y);
	
	float* map_sample = createTexCoords(texture, sprite[TILE_CLOSED], sprite_size);
	ASSERT_LOG(map_sample, "Could not load subtexture!");
	
	if(s -> reveal_state[y][x] == TILE_CLOSED || s -> reveal_state[y][x] == TILE_UNFLAGGED)
	{	
		DrawQuad(renderer, p_pos, p_size, texture, map_sample);
		return;
	}
	
	if(s -> reveal_state[y][x] == TILE_FLAGGED)
	{
		DrawQuad(renderer, p_pos, p_size, texture, map_sample);
		map_sample = createTexCoords(texture, sprite[FLAG_TILE], sprite_size);
		ASSERT_LOG(map_sample, "Could not load subtexture!");
	} 
	else if(s -> board[y][x] != BOMB)
	{
		if(s -> board[y][x] == EMPTY) return;
		map_sample = createTexCoords(texture, sprite[s -> board[y][x]], sprite_size);
		ASSERT_LOG(map_sample, "Could not load subtexture!");
	}
	else
	{
		map_sample = createTexCoords(texture, sprite[BOMB_TILE], sprite_size);
		ASSERT_LOG(map_sample, "Could not load subtexture!");
	}
	
	DrawQuad(renderer, p_pos, p_size, texture, map_sample);
}

//This is an overlay that tells the user that the game is now complete and gives them the option to restart
static void draw_overlay(struct Renderer* renderer, bool game_over_overlay) 
{	
	// Dimensions for the white box
	vec2 boxPos = {-10.0f/23.0f, -0.5f};  // Centered on screen
	vec2 boxSize = {20.0f/23.0f, 14.0f/23.0f};   // Size of the box
	
	if(game_over_overlay)
		DrawQuad(renderer, boxPos, boxSize, game_over, FULL_SAMPLE);
	else
		DrawQuad(renderer, boxPos, boxSize, win_overlay, FULL_SAMPLE);
	
	// Dimensions for the button
	vec2 buttonPos = {-5.0f/23.0f, -0.4f};
	vec2 buttonSize = {10.0f/23.0f, 3.0f/23.0f};
	DrawQuad(renderer, buttonPos, buttonSize, button_restart, FULL_SAMPLE);
}

static void draw_board(GameState* s, struct Renderer* renderer, uint32_t texture)
{
	uint8_t rows = s -> tiles_y;
	uint8_t cols = s -> tiles_x;
	
	ASSERT_LOG(renderer, "Attempting to draw on an uninitialised renderer!");
	
	float norm_grid_offset_x, 	norm_grid_offset_y;
	float norm_tile_size_x, 	norm_tile_size_y;						  
	float norm_spacing_x, 		norm_spacing_y;
	
	calc_norms(s, (float* []){&norm_grid_offset_x, &norm_grid_offset_y, &norm_tile_size_x,
			&norm_tile_size_y, &norm_spacing_x, &norm_spacing_y}); //tiling function
	
	float xpos, ypos;
	
	vec2 p_pos, p_size = {norm_tile_size_x, norm_tile_size_y};

	// Loop to render tiles
	for (uint8_t i = 1; i < rows + 1; i++) 
	{
		for (uint8_t j = 0; j < cols; j++) 
		{			
			// Compute normalized tile position
			xpos = s -> config_pos[0] + norm_grid_offset_x + norm_spacing_x + j * (norm_tile_size_x + norm_spacing_x);
			ypos = -s -> config_pos[1] - norm_grid_offset_y - i * (norm_tile_size_y + norm_spacing_y);

			// Normalized size for the tile
			p_pos[0] = xpos;
			p_pos[1] = ypos;

			draw_piece(s, renderer, texture, j, i - 1, p_pos, p_size);
		}
	}
}

static void board_controller(GameState* s, struct Renderer* renderer, int choose_scene) 
{
   	s -> config_pos[0] = scenes[choose_scene].pos[0];
	s -> config_pos[1] = scenes[choose_scene].pos[1];

	s -> config_size[0] = scenes[choose_scene].size[0];
	s -> config_size[1] = scenes[choose_scene].size[1];
	s -> config_texture = scenes[choose_scene].texture;

	DrawColour(renderer, (vec2){-1.0f, -1.0f}, (vec2){2.0f, 2.0f}, (vec4){0.57f, 0.51f, 0.55f, 1.0f});
    DrawQuad(renderer, s -> config_pos, s -> config_size, s -> config_texture, FULL_SAMPLE);
	draw_board(s, renderer, board_pieces);
}

// controls which scene the game is in 
void scene_controller(GameState* s, struct Renderer* renderer)
{
	ASSERT_LOG(renderer, "Attempting to draw on an uninitialised renderer!");

	s -> config_pos[0] = scenes[DEFAULT].pos[0];
	s -> config_pos[1] = scenes[DEFAULT].pos[1];

	s -> config_size[0] = scenes[DEFAULT].size[0];
	s -> config_size[1] = scenes[DEFAULT].size[1];
	s -> config_texture = scenes[DEFAULT].texture;

	vec3 bg_colour = {0.57, 0.51, 0.55};
		
	switch(s -> scene)
	{
		case (MENU):
			resetOrthoCamera();
			DrawQuad(renderer, s -> config_pos, s -> config_size, menu_texture, FULL_SAMPLE);
			return;
		case (OPTIONS):
			DrawQuad(renderer, s -> config_pos, s -> config_size, options_texture, FULL_SAMPLE);
			return;
		case (EASY_BOARD):
			setClearColour(bg_colour);
			board_controller(s, renderer, EASY);
			break;
		case (INTER_BOARD):
			setClearColour(bg_colour);
			board_controller(s, renderer, INTERMEDIATE);
			break;
		case (HARD_BOARD):
			setClearColour(bg_colour);
			board_controller(s, renderer, HARD);
			break;
	}
	
	switch(s -> game_state)
	{
		case (GAME_OVER):
			draw_overlay(renderer, s -> game_over_overlay);
			break;
		case (RESTART):
			REYNOLDS_DEBUG("@instance: reset implemented");
			reset_board(s);
			
			s -> win 			= false;
			s -> lose 			= false;
			s -> init_state		= true;
			s -> game_state 	= PLAY;
			break;
	}
}