#include "OrthoCameraControllerInternals.h"

#include <GLFW/glfw3.h>
#include "window.h"
#include "events/event.h"
#include "events/mouse_event.h"

#include "input.h"
#include "logging.h"

OrthographicCameraController controller;

float camera_rotation = 0.0f;
float original_ar, minimum_zoom = 0.0f, maximum_zoom = 0.0f, left_bounds = 0.0f, right_bounds = 0.0f;

void setLateralLimits(float left, float right)
{
	left_bounds = left;
	right_bounds = right;
}

static void checkLateralLimits(OrthoCamera* camera)
{
	camera -> position[0] = MAX(left_bounds, camera -> position[0]);
	camera -> position[0] = MIN(right_bounds, camera -> position[0]);
}

void setZoomLimits(float min_zoom, float max_zoom)
{
	minimum_zoom = min_zoom;
	maximum_zoom = max_zoom;
}

void checkZoomLimits()
{
	float zoom_level = controller.zoom_level;

	controller.zoom_level = MAX(minimum_zoom, controller.zoom_level);

	//If zoom does not reach the maximum zoom level, see if it reached the minimum zoom level
	if(controller.zoom_level == zoom_level)
		controller.zoom_level = MIN(maximum_zoom, controller.zoom_level);
}

void createOrthoCameraController(float aspect_ratio)
{	
	controller.zoom_level 				= 1.0f;
	controller.camera_translation_speed = 5.0f;
	controller.camera_rotation_speed 	= 180.0f;
	controller.aspect_ratio 			= aspect_ratio;
	controller.translation				= true;
	controller.rotation 				= false;
	controller.inverted					= false;


	float zoom_level = controller.zoom_level;
	createOrthoCamera(-aspect_ratio*zoom_level, aspect_ratio*zoom_level, 
					-zoom_level, zoom_level);
	
	original_ar = aspect_ratio;
}

void resetOrthoCamera()
{
	controller.aspect_ratio = original_ar;
	controller.zoom_level 	= 1.0f;
	
	OrthoCamera* camera = &controller.camera;
	
	camera -> position[0] 	= 0;
	camera -> position[1]	= 0;
	camera -> rotation 		= 0;
	
	float aspect_ratio = controller.aspect_ratio;
	float zoom_level = controller.zoom_level;
	
	setOrthoProjection(-aspect_ratio*zoom_level, aspect_ratio*zoom_level, 
					-zoom_level, zoom_level);

	setOrthoPosition(camera -> position, false);
	setOrthoRotation(camera -> rotation, true);
}

void invert_camera(bool inverted) { controller.inverted = inverted; }
void enable_translation(bool enable) { controller.translation = enable; }
void enable_rotation(bool enable) { controller.rotation = enable; }
void enable_zoom(bool enable) { controller.zoom = enable; }
void enable_camera(bool enable)
{
	enable_translation(enable);
	enable_zoom(enable);
	enable_rotation(enable);
}

float getZoomLevel() { return controller.zoom_level; }
float getAspectRatio() { return controller.aspect_ratio; }

float* getCameraPosition()
{
    return (float*) getOrthoPosition();
}

void onUpdate(float ts)
{
	float camera_translation_speed 	= controller.camera_translation_speed;
	float camera_rotation_speed 	= controller.camera_rotation_speed;
	OrthoCamera* camera 			= &controller.camera;
	
	if(controller.rotation)
	{
		if(isKeyPressed(GLFW_KEY_Q))
			camera -> rotation += camera_rotation_speed * ts;
		else if(isKeyPressed(GLFW_KEY_E))
			camera -> rotation -= camera_rotation_speed * ts;
		
		setOrthoRotation(camera -> rotation, true);
	}

	if(!controller.translation)
		return;

	if(!controller.inverted)
	{
		if(isKeyPressed(GLFW_KEY_W))
			camera -> position[1] -= camera_translation_speed * ts; //may want to make translation speed a function of zoom_level later
		else if(isKeyPressed(GLFW_KEY_S))                    
			camera -> position[1] += camera_translation_speed * ts;
		else if(isKeyPressed(GLFW_KEY_D))                     
			camera -> position[0] -= camera_translation_speed * ts;
		else if(isKeyPressed(GLFW_KEY_A))                     
			camera -> position[0] += camera_translation_speed * ts;
	} else {
		if(isKeyPressed(GLFW_KEY_W))
			camera -> position[1] += camera_translation_speed * ts; //may want to make translation speed a function of zoom_level later
		else if(isKeyPressed(GLFW_KEY_S))                    
			camera -> position[1] -= camera_translation_speed * ts;
		else if(isKeyPressed(GLFW_KEY_D))                     
			camera -> position[0] += camera_translation_speed * ts;
		else if(isKeyPressed(GLFW_KEY_A))                     
			camera -> position[0] -= camera_translation_speed * ts;	
	}
	
	checkLateralLimits(camera);

	controller.camera_translation_speed = controller.zoom_level;
	setOrthoPosition(camera -> position, true);
}	

static void onMouseScrolled()
{
	if(controller.zoom)
	{
		controller.zoom_level -= (float)getYOffset()*0.25f;

		if(!(minimum_zoom && maximum_zoom))
		{
			REYNOLDS_ERROR("Zoom limits not set!");
			return;
		}

		checkZoomLimits();

		float aspect_ratio 	= controller.aspect_ratio;
		float zoom_level 	= controller.zoom_level;
		
		setOrthoProjection(-aspect_ratio*zoom_level, aspect_ratio*zoom_level, 
						-zoom_level, zoom_level);
	}
}

static void onWindowResize()
{
	controller.aspect_ratio = (float)getWindowWidth()/(float)getWindowHeight();

	float aspect_ratio 	= controller.aspect_ratio;
	float zoom_level 	= controller.zoom_level;

	setOrthoProjection(-aspect_ratio*zoom_level, aspect_ratio*zoom_level, 
					-zoom_level, zoom_level);
}

//mouse scrolled changes the zoom level
//window resize changes the aspect ratio
static void onEvent(Event* e)
{
	if(e -> type == MouseScroll) { onMouseScrolled(); e -> handled = true; }
	else if(e -> type == WindowResize) { onWindowResize(); }
}

Layer create_camera_layer()
{
	  Layer camera_layer = {
        .name = "Camera Layer",
		.id = LAYER_CAMERA,
        .onEvent = onEvent,
		.update = onUpdate
    };
    return camera_layer;
}