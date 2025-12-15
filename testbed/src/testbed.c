#include "app_input.h"

#include "common.h"
#include <engine.h>
#include <time.h>

#include "board.h"
#include "game_logic.h"
#include "display.h"
#include "gamestate.h"
#include "gameplay_layer.h"

const char* ICON_PATH 	= "../testbed/res/extra/minesweeper.png";
const char* SHADER_PATH = "../testbed/res/shaders/shader";

int main()
{
	set_log_level(LOG_LEVEL_VERBOSE);
	REYNOLDS_INFO("Initialising Log...");
	
	Window* window = create_window("minesweeper.c", SCREEN_WIDTH, SCREEN_HEIGHT);
	SetEventCallback(&onEvent);

	//disable_layer_event(&camera_layer);
	load_icon(ICON_PATH);
	
	/* Initialising GLEW*/
	GLenum err = glewInit();
	ASSERT_LOG(err == GLEW_OK, "GLEW Error: %s", glewGetErrorString(err));
	
	vendor_dependencies();

	setShaderPath(SHADER_PATH);	
	
	GameState state = {0};
	state.scene 	= MENU;
	state.init_state = true;

	/* Initialise and load stack defined renderer */
	struct Renderer st_render; 

	stack = create_layer_stack();
	Layer render_layer 			= create_render_layer(&st_render);
	Layer camera_layer 			= create_camera_layer();
	Layer gameplay_layer 		= create_gameplay_layer(&state);
	Layer close_window_layer 	= {"Window Close Layer", .id = LAYER_WINDOW_CLOSE, .onEvent = onWindowClose};

	push_layer(stack, render_layer);
	push_layer(stack, close_window_layer);
	push_layer(stack, gameplay_layer);
	push_layer(stack, camera_layer);

	float aspect_ratio = (float)getWindowWidth()/(float)getWindowHeight();
	createOrthoCameraController(aspect_ratio);
	setZoomLimits(0.25f, 1.0f);

	invert_camera(true);
	enable_camera(false);

	setClearColour((vec3){1.0f, 1.0f, 1.0f}); // NOTE: set to white
	
	struct Statistics stats;
	bool handled = false;
	
	float timestep;
	srand(time(NULL));

	/* Loop until the user closes the window */
    while (Running)
    {	
		timestep = get_delta_time();	
		resetStats();
		render_clear();

		if(state.game_state == PLAY)
		{
			enable_camera(true);
			enable_rotation(false);
			setLateralLimits(-0.5f, 0.5f);
		}

		if(!Minimised)
		{
			setMat4("u_ProjectionView", getPVMat());
			
			BeginBatch(&st_render);
			
			MouseInput(&state);
			KeyInput(&state);

			update_layers(stack, timestep);
			scene_controller(&state, &st_render);
			
			EndBatch(&st_render);
			FlushBatch(&st_render); //combine both and name 'ExitBatch' or 'ExecuteBatch'
		}
		
        update_window();
		stats = getRenderStats();
		if(isKeyPressed(GLFW_KEY_L))
		{
			if(!handled)
			{
				REYNOLDS_DEBUG("Draw calls: %d\tQuad count: %d", stats.DrawCalls, stats.QuadCount);
				handled = true;
			}
		} else
			handled = false;
    }
	
	close_window();
	
	if(state.board)
		delete_board(&state);
	
    return 0;
}