#include <engine.h>
#include <Camera/camera.h>
#include <time.h>

#include "board.h"
#include "common.h"
#include "gameplay_layer.h"
#include "ui_layer.h"
#include "debug_layer.h"
#include "gamestate.h"
#include "display.h"

const char* ICON_PATH 	= "../testbed/res/extra/minesweeper.png";
const char* SHADER_PATH = "../testbed/res/shaders/shader";

struct Statistics stats;

int main()
{
	/** BOOTSTRAP PHASE **/	
	set_log_level(LOG_LEVEL_VERBOSE);
	InitEngine("minesweeper.c", SCREEN_WIDTH, SCREEN_HEIGHT);
    EngineDependencies();
	LoadIcon(ICON_PATH);

	DisplayContext context 	= {0};
	GameState state 		= {0};
	state.scene 			= MENU;

	/* Initialise and load stack defined renderer */
	struct Renderer* renderer 	= renderer_create(); 
	struct Shader* batch_shader = LoadShader(SHADER_PATH);

	SetShader(renderer, batch_shader);
	//load uniform textures
	int samplers[MAX_TEXTURE_SLOTS];
	for (int i = 0; i < MAX_TEXTURE_SLOTS; i++)
		samplers[i] = i;
		
	SetIntArray(batch_shader, "u_textures", samplers, MAX_TEXTURE_SLOTS);

	struct Font* font = LoadFont("../testbed/res/fonts/cmb10.ttf", 48.0f);
	setFont(font);

	struct UIContext* ui = UICreate(renderer, font);

	/* Initialise Layer Stack and Layers */
	LayerStack* stack 			= InitLayerStack();
	Layer render_layer 			= create_render_layer(renderer);
	Layer camera_layer 			= create_camera_layer();
	Layer gameplay_layer 		= create_gameplay_layer(renderer, &context, &state); //UNCLEAR: audio layer in gameplay layer or separate from?
	Layer ui_layer 				= create_ui_layer(renderer, &context, &state, ui);
	Layer close_window_layer 	= {"Window Close Layer", .id = LAYER_WINDOW_CLOSE, .onEvent = onWindowClose};
	Layer debug_layer 			= create_debug_layer(renderer);
	
	push_layer(stack, render_layer);
	push_layer(stack, close_window_layer);
	push_layer(stack, gameplay_layer);
	push_layer(stack, ui_layer);
	push_layer(stack, camera_layer);
	push_layer(stack, debug_layer);
	
	/* Setup orthographic camera */
	float aspect_ratio = (float)getWindowWidth()/(float)getWindowHeight();

	createOrthoCameraController(aspect_ratio);
	invert_camera(true); enable_translation(true); enable_zoom(true);
	setZoomLimits(0.25f, 1.0f);
	setLateralLimits(-0.5f, 0.5f);

	/* Scene setup*/
	init_display(&context);
	init_scenes(&context, &state);
	SetClearColour((vec3){1.0f, 1.0f, 1.0f}); // NOTE: set to white #ffffff

	//audio_init();
	//audio_play_music("../testbed/res/audio/track_B.wav");

	/** RUNTIME PHASE **/
    EngineRun(renderer, stack);

	//** SHUTDOWN PHASE **//

	unload_textures(&context);
	
	if(state.board)
		delete_board(state.board);
	
	EngineShutdown();
	audio_shutdown();
	UIDestroy(ui);
	FreeShader(batch_shader);
	FreeFont(font);
    return 0;
}