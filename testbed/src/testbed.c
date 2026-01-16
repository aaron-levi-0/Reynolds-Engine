#include <engine.h>
#include <time.h>

#include "board.h"
#include "common.h"
#include "gameplay_layer.h"
#include "gamestate.h"
#include "display.h"

const char* ICON_PATH 	= "../testbed/res/extra/minesweeper.png";
const char* SHADER_PATH = "../testbed/res/shaders/shader";

int main()
{
	/** BOOTSTRAP PHASE **/

	set_log_level(LOG_LEVEL_VERBOSE);
	REYNOLDS_INFO("Initialising Log...");
	
	EngineInit("minesweeper.c", SCREEN_WIDTH, SCREEN_HEIGHT);
    EngineDependencies();

	load_icon(ICON_PATH);
	SetShaderPath(SHADER_PATH);	

	DisplayContext context = {0};

	GameState state 	= {0};
	state.scene 		= MENU;

	/* Initialise and load stack defined renderer */
	struct Renderer* renderer = renderer_create(); 

	/* Initialise Layer Stack and Layers */
	LayerStack* stack 			= InitLayerStack();
	Layer render_layer 			= create_render_layer(renderer);
	Layer camera_layer 			= create_camera_layer();
	Layer gameplay_layer 		= create_gameplay_layer(renderer, &context, &state);
	Layer close_window_layer 	= {"Window Close Layer", .id = LAYER_WINDOW_CLOSE, .onEvent = onWindowClose};

	push_layer(stack, render_layer);
	push_layer(stack, close_window_layer);
	push_layer(stack, gameplay_layer);
	push_layer(stack, camera_layer);

	// disable_layer_event(&gameplay_layer); // disable gameplay events for debugging or ui //
	
	/* Setup orthographic camera */
	float aspect_ratio = (float)getWindowWidth()/(float)getWindowHeight();
	createOrthoCameraController(aspect_ratio);
	setZoomLimits(0.25f, 1.0f);

	invert_camera(true);
	enable_translation(true);
	enable_zoom(true);
	setLateralLimits(-0.5f, 0.5f); //or do enable_camera_events
	disable_layer_event(&camera_layer);

	/* Misc */
	struct Statistics stats;
	bool key_event_handled = false;
	
	float timestep;
	srand(time(NULL));
	
	init_display(&context);
	init_scenes(&context, &state);

	SetClearColour((vec3){1.0f, 1.0f, 1.0f}); // NOTE: set to white #ffffff

	/** RUNTIME PHASE **/

    while (Running())
    {	
		timestep = get_delta_time();	
		resetStats(renderer);

		if(!Minimised())
		{
			render_clear();

			if(state.game_state == PLAY && camera_layer.event_enabled == false)
				enable_layer_event(&camera_layer);
			else if(state.scene == MENU && camera_layer.event_enabled == true)
				disable_layer_event(&camera_layer); //this doesnt do anything apparently

			setMat4("u_ProjectionView", getPVMat());
			
			BeginBatch(renderer);

			update_layers(stack, timestep);
			render_layers(stack);

			EndBatch(renderer);
			FlushBatch(renderer); //combine both and name 'ExitBatch' or 'ExecuteBatch'

			update_window();

			if(isKeyPressed(GLFW_KEY_L))
			{
				if(!key_event_handled)
				{
					REYNOLDS_DEBUG("Draw calls: %d\tQuad count: %d", stats.DrawCalls, stats.QuadCount);
					key_event_handled = true;
				}
			} else key_event_handled = false;
		}
		getRenderStats(renderer, &stats);
    }
	
	//** SHUTDOWN PHASE **//

	unload_textures(&context);
	close_window();
	
	if(state.board)
		delete_board(&state);
	
	renderer_destroy(renderer);
    return 0;
}