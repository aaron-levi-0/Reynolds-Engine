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
	
	engine_init("minesweeper.c", SCREEN_WIDTH, SCREEN_HEIGHT);
    vendor_dependencies();

	load_icon(ICON_PATH);
	setShaderPath(SHADER_PATH);	

	GameState state 	= {0};
	state.scene 		= MENU;

	/* Initialise and load stack defined renderer */
	struct Renderer st_render; 

	/* Initialise Layer Stack and Layers */
	stack = create_layer_stack();
	Layer render_layer 			= create_render_layer(&st_render);
	Layer camera_layer 			= create_camera_layer();
	Layer gameplay_layer 		= create_gameplay_layer(&state);
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
	enable_camera(false);
	
	/* Misc */
	struct Statistics stats;
	bool handled = false;
	
	float timestep;
	srand(time(NULL));
	
	init_textures();
	init_scenes();

	setClearColour((vec3){1.0f, 1.0f, 1.0f}); // NOTE: set to white #ffffff

	/** RUNTIME PHASE **/

    while (Running)
    {	
		timestep = get_delta_time();	
		resetStats();

		if(!Minimised)
		{
			render_clear();

			if(state.game_state == PLAY)
			{
				enable_camera(true);
				enable_rotation(false);
				setLateralLimits(-0.5f, 0.5f);
			}

			setMat4("u_ProjectionView", getPVMat());
			
			BeginBatch(&st_render);

			update_layers(stack, timestep);
			scene_render(&state, &st_render);
			
			EndBatch(&st_render);
			FlushBatch(&st_render); //combine both and name 'ExitBatch' or 'ExecuteBatch'

			update_window();

			if(isKeyPressed(GLFW_KEY_L))
			{
				if(!handled)
				{
					REYNOLDS_DEBUG("Draw calls: %d\tQuad count: %d", stats.DrawCalls, stats.QuadCount);
					handled = true;
				}
			} else handled = false;
		}
		stats = getRenderStats();
    }
	
	//** SHUTDOWN PHASE **//

	unload_textures();
	close_window();
	
	if(state.board)
		delete_board(&state);
	
    return 0;
}