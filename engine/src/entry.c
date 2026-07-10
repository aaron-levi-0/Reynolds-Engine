#include <GL/glew.h>
#include <time.h>

#include "entry_internals.h"
#include "core/layers_internals.h"
#include "core/window_internals.h"
#include "renderer/renderer_internals.h"

#include "logging.h"
#include "shader.h"
#include "timestep.h"
#include "Camera/OrthographicCamera.h"

LayerStack* stack = NULL;

static bool isRunning 	= true;
static bool isMinimised = false;

bool Running()
{
	return isRunning;
}

bool Minimised()
{
	return isMinimised;
}

void onEvent(Event* e)
{
	VALIDATE_LOG(e);
	VALIDATE_LOG(!e -> handled);
	stack = handle_layer_events(stack, e);
}

void onWindowClose(Event* e)
{
	if(e -> type == WindowClose)
	{
		isRunning = false;
		request_window_close = true;
	}
}

void InitEngine(const char* title, int w, int h)
{
	ASSERT_FATAL(title, "@engine: window title is NULL!");
	ASSERT_FATAL(w > 0 && h > 0, "@engine: invalid window dimensions!");

	REYNOLDS_INFO("Initialising Log...");

    create_window(title, w, h);
	SetEventCallback(&onEvent);

	/* Initialising GLEW*/
    GLenum err = glewInit();
    ASSERT_FATAL(err == GLEW_OK, "GLEW Error: %s", glewGetErrorString(err));
}

LayerStack* InitLayerStack()
{
	stack = create_layer_stack();
	ASSERT_FATAL(stack, "@engine: failed to create layer stack!");
	
	return stack;
}

void EngineRun(struct Renderer* renderer, LayerStack* stack)
{
	/* Time setup */
	float timestep;
	srand(time(NULL));

	while (Running())
    {	
		timestep = get_delta_time();	
		resetStats(renderer);

		if(!Minimised())
		{
			render_clear();		
			setViewProjection(renderer, getPVMat());

			BeginBatch(renderer);
			update_layers(stack, timestep);
			render_layers(stack);
			EndBatch(renderer);

			FlushBatch(renderer);
			update_window();
		}
    }
}

void EngineShutdown()
{
	close_window();
	destroy_layer_stack(stack);
}