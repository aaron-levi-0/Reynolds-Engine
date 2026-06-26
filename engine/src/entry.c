#include <GL/glew.h>

#include "entry_internals.h"
#include "core/layers_internals.h"
#include "core/window_internals.h"

#include "logging.h"

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
	e -> handled = true;
}

void onWindowClose(Event* e)
{
	if(e -> type == WindowClose)
	{
		isRunning = false;
		request_window_close = true;
	}
}

void EngineInit(const char* title, int w, int h)
{
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

void EngineShutdown()
{
	close_window();
	destroy_layer_stack(stack);
}