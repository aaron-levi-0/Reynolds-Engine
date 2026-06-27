#include "debug_layer.h"

#include <GLFW/glfw3.h>
#include "logging.h"
#include "input.h"

static struct Renderer* st_render = NULL;

void debug_update(float deltaTime)
{
	if(isKeyPressed(GLFW_KEY_F))
		REYNOLDS_DEBUG("FPS: %d", (int)(1.0f/deltaTime));

	if(isKeyPressed(GLFW_KEY_L))
		REYNOLDS_DEBUG("Draw calls: %d\tQuad count: %d", getDrawCalls(st_render), getQuadCount(st_render));
}

Layer create_debug_layer(struct Renderer* r) {
    st_render = r;
    return (Layer){ .name="Debug Layer", .id=LAYER_DEBUG, .update=debug_update };
}