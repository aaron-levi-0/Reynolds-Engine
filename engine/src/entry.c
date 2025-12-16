#include "entry.h"
#include "core/window.h"

LayerStack* stack = NULL;

bool Running 	= true;
bool Minimised 	= false;

void onEvent(Event* e)
{
	VALIDATE(e);
	VALIDATE(!e -> handled);
	handle_layer_events(stack, e);
	e -> handled = true;
}

void onWindowClose(Event* e)
{
	if(e -> type == WindowClose)
	{
		Running = false;
		destroy_layer_stack(stack);
	}
}

void engine_init(const char* title, int w, int h)
{
    create_window(title, w, h);
	SetEventCallback(&onEvent);

	/* Initialising GLEW*/
    GLenum err = glewInit();
    ASSERT_LOG(err == GLEW_OK, "GLEW Error: %s", glewGetErrorString(err));
}