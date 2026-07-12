#include "debug_layer.h"

#include "logging.h"
#include "input.h"
#include "keycodes.h"

static struct Renderer* st_render = NULL;

void debug_update(float deltaTime)
{
	if(isKeyPressed(KEY_F))
		REYNOLDS_DEBUG("FPS: %d", (int)(1.0f/deltaTime));

	if(isKeyPressed(KEY_L))
		REYNOLDS_DEBUG("Draw calls: %d\tQuad count: %d", getDrawCalls(st_render), getQuadCount(st_render));
}

void debug_render()
{
	if(isKeyPressed(KEY_F))
		return;
}

Layer create_debug_layer(struct Renderer* r) 
{
    st_render = r;
    return (Layer){ 
		.name = "Debug Layer", 
		.id = LAYER_DEBUG, 
		.update = debug_update,
		.render = debug_render 
	};
}