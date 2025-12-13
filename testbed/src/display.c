#include "display.h"

#include "common.h"
#include "game_logic.h"
#include "board.h"

#include <renderer/texture.h>
#include <OrthoCameraController.h>

bool init_state = true;
bool game_over_overlay;

const float MAP_OFFSET	= 46.0f; 	//NOTE: texture map subtexture offset (in pixels)

vec4 FULL_SAMPLE = {0.0f, 0.0f, 1.0f, 1.0f};

uint32_t menu_texture, options_texture;
uint32_t easy_board, inter_board, hard_board;
uint32_t board_pieces, button_restart, game_over, win_overlay;

int game_state 	= NONE;
int scene 		= MENU;

typedef struct 
{
    vec2 pos, size;
    uint32_t* texture;
} SceneConfig;

enum choose_scene { EASY, INTERMEDIATE, HARD };

#define DEFAULT	(INTERMEDIATE)

SceneConfig scenes[] = {
	[EASY] 			= {{-0.5f, -0.5f}, {1.0f, 1.0f}, &easy_board}, 	//TO-DO: stop zoom-in making tex blurry
    [INTERMEDIATE] 	= {{-1.0f, -1.0f}, {2.0f, 2.0f}, &inter_board},	//NOTE: default
	[HARD] 			= {{-0.75f, -0.4f}, {1.5f, 0.8f}, &hard_board}
};

SceneConfig current_config;

float* getConfigPosition()
{
	return current_config.pos;
}

float* getConfigSize()
{
	return current_config.size;
}

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

static void draw_piece(struct Renderer* renderer, uint32_t texture, int x, int y, vec2 p_pos, vec2 p_size)
{
	int** board 		= getBoardState();
	int** reveal_state 	= getRevealState();
	
	VALIDATE_LOG(renderer, "Attempting to draw on an uninitialised renderer!");
	VALIDATE_LOG(board, "Application trying to access non-existant board memory!");
	VALIDATE_LOG(reveal_state, "Application is trying to access non-existant board memory!");
	VALIDATE_LOG(valid_tile(x, y), "Invalid board coordinates: (%d, %d)", x, y);
	
	float* map_sample = createTexCoords(texture, sprite[TILE_CLOSED], sprite_size);
	ASSERT_LOG(map_sample, "Could not load subtexture!");
	
	if(reveal_state[y][x] == TILE_CLOSED || reveal_state[y][x] == TILE_UNFLAGGED)
	{	
		DrawQuad(renderer, p_pos, p_size, texture, map_sample);
		return;
	}
	
	if(reveal_state[y][x] == TILE_FLAGGED)
	{
		DrawQuad(renderer, p_pos, p_size, texture, map_sample);
		map_sample = createTexCoords(texture, sprite[FLAG_TILE], sprite_size);
		ASSERT_LOG(map_sample, "Could not load subtexture!");
	} 
	else if(board[y][x] != BOMB)
	{
		if(board[y][x] == EMPTY) return;
		map_sample = createTexCoords(texture, sprite[board[y][x]], sprite_size);
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

static void draw_board(struct Renderer* renderer, uint32_t texture)
{
	uint8_t rows = getBoardRows();
	uint8_t cols = getBoardCols();
	
	ASSERT_LOG(renderer, "Attempting to draw on an uninitialised renderer!");
	
	float norm_grid_offset_x, 	norm_grid_offset_y;
	float norm_tile_size_x, 	norm_tile_size_y;						  
	float norm_spacing_x, 		norm_spacing_y;
	
	calc_norms(current_config.size, (float* []){&norm_grid_offset_x, &norm_grid_offset_y, &norm_tile_size_x,
			&norm_tile_size_y, &norm_spacing_x, &norm_spacing_y}); //tiling function
	
	float xpos, ypos;
	
	vec2 p_pos, p_size = {norm_tile_size_x, norm_tile_size_y};

	// Loop to render tiles
	for (uint8_t i = 1; i < rows + 1; i++) 
	{
		for (uint8_t j = 0; j < cols; j++) 
		{			
			// Compute normalized tile position
			xpos = current_config.pos[0] + norm_grid_offset_x + norm_spacing_x + j * (norm_tile_size_x + norm_spacing_x);
			ypos = -current_config.pos[1] - norm_grid_offset_y - i * (norm_tile_size_y + norm_spacing_y);

			// Normalized size for the tile
			p_pos[0] = xpos;
			p_pos[1] = ypos;

			draw_piece(renderer, texture, j, i - 1, p_pos, p_size);
		}
	}
}

static void board_controller(struct Renderer* renderer, int choose_scene) 
{
    current_config = scenes[choose_scene];
	
	DrawColour(renderer, (vec2){-1.0f, -1.0f}, (vec2){2.0f, 2.0f}, (vec4){0.57f, 0.51f, 0.55f, 1.0f});
    DrawQuad(renderer, current_config.pos, current_config.size, current_config.texture[0], FULL_SAMPLE);
	draw_board(renderer, board_pieces);
}

// controls which scene the game is in 
void scene_controller(struct Renderer* renderer)
{
	ASSERT_LOG(renderer, "Attempting to draw on an uninitialised renderer!");

	current_config = scenes[DEFAULT];
	float* position = current_config.pos;
	float* size = current_config.size;
	vec3 bg_colour = {0.57, 0.51, 0.55};
		
	switch(scene)
	{
		case (MENU):
			resetOrthoCamera();
			DrawQuad(renderer, position, size, menu_texture, FULL_SAMPLE);
			return;
		case (OPTIONS):
			DrawQuad(renderer, position, size, options_texture, FULL_SAMPLE);
			return;
		case (EASY_BOARD):
			setClearColour(bg_colour);
			board_controller(renderer, EASY);
			break;
		case (INTER_BOARD):
			setClearColour(bg_colour);
			board_controller(renderer, INTERMEDIATE);
			break;
		case (HARD_BOARD):
			setClearColour(bg_colour);
			board_controller(renderer, HARD);
			break;
	}
	
	switch(game_state)
	{
		case(PLAY):
			win = check_win();
			break;
		case (GAME_OVER):
			draw_overlay(renderer, game_over_overlay);
			break;
		case (RESTART):
			REYNOLDS_DEBUG("@instance: reset implemented");
			reset_board();
			
			win 		= false;
			lose 		= false;
			init_state	= true;
			game_state 	= PLAY;
			break;
	}
}