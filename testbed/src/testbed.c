#include "app_input.h"

#include "common.h"
#include <engine.h>
#include <time.h>

#include "board.h"
#include "game_logic.h"
#include "display.h"
#include "gamestate.h"

const char* ICON_PATH 	= "../testbed/res/extra/minesweeper.png";
const char* SHADER_PATH = "../testbed/res/shaders/shader";

int main()
{
	set_log_level(LOG_LEVEL_VERBOSE);
	REYNOLDS_INFO("Initialising Log...");
	
	Window* window = create_window("minesweeper.c", SCREEN_WIDTH, SCREEN_HEIGHT);
	SetEventCallback(&onEvent);

	stack = create_layer_stack();
	Layer render_layer = create_render_layer();
	Layer camera_layer = create_camera_layer();
	Layer close_window_layer = {"Window Close Layer", .id = WINDOW_CLOSE, .onEvent = onWindowClose};

	push_layer(stack, render_layer);
	push_layer(stack, close_window_layer);
	push_layer(stack, camera_layer);

	//disable_layer_event(&camera_layer);
	load_icon(ICON_PATH);
	
	/* Initialising GLEW*/
	GLenum err = glewInit();
	ASSERT_LOG(err == GLEW_OK, "GLEW Error: %s", glewGetErrorString(err));
	
	vendor_dependencies();
	
	/* Initialise and load stack defined renderer */
	struct Renderer st_render; 
	
	GameState state = {0};
	state.scene 	= MENU;

	setShaderPath(SHADER_PATH);

	render_init(&st_render);
	MallocDraw(&st_render);	
	
	init_textures();
	init_scenes();
	
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
		
		if (state.game_state == PLAY)
    		state.win = check_win(&state);

		if(!Minimised)
		{
			setMat4("u_ProjectionView", getPVMat());
			
			BeginBatch(&st_render);
			
			setMousePos();
			MouseInput(&state);
			KeyInput(&state);

			update_layers(stack, timestep);
			
			if(state.game_state == PLAY)
			{
				enable_camera(true);
				enable_rotation(false);

				setLateralLimits(-0.5f, 0.5f);
				
			}
			scene_controller(&state, &st_render);
					
			if(state.win || state.lose)
			{
				state.game_state = GAME_OVER;
				state.game_over_overlay = ! state.win;
			}
			
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
	
	FreeDraw(&st_render);
	close_window();
	
	if(state.board)
		delete_board(&state);
	
    return 0;
}