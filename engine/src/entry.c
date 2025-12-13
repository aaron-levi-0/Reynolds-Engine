#include "entry.h"

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