#include <GL/glew.h>

#include "entry.h"
#include "core/layers_internals.h"
#include "../include/window.h"

#include "../include/logging.h"

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
	handle_layer_events(stack, e);
	e -> handled = true;
}

void onWindowClose(Event* e)
{
	if(e -> type == WindowClose)
	{
		isRunning = false;
		destroy_layer_stack(stack);
	}
}

void engine_init(const char* title, int w, int h)
{
    create_window(title, w, h);
	SetEventCallback(&onEvent);

	/* Initialising GLEW*/
    GLenum err = glewInit();
    ASSERT_FATAL(err == GLEW_OK, "GLEW Error: %s", glewGetErrorString(err));
}