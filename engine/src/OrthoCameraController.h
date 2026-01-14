#ifndef ORTHOCAMERACONTROLLER_H
#define ORTHOCAMERACONTROLLER_H

#include "core/layers.h"
#include "core/input.h"
#include "renderer/OrthographicCamera.h"

typedef struct {
	float aspect_ratio;
	float zoom_level;
	OrthoCamera camera;
	
	bool translation;
	bool rotation;
	bool zoom;
	bool inverted;
	
	float camera_translation_speed;
	float camera_rotation_speed;
} OrthographicCameraController;

REN_API void createOrthoCameraController(float);
REN_API Layer create_camera_layer();

REN_API void resetOrthoCamera();

REN_API void invert_camera(bool ); 
REN_API void enable_translation(bool );
REN_API void enable_rotation(bool ); 
REN_API void enable_zoom(bool );
REN_API void enable_camera(bool );

REN_API void setZoomLimits(float , float );
REN_API void setLateralLimits(float , float );
REN_API float getZoomLevel();
REN_API float getAspectRatio();
REN_API float* getCameraPosition();
REN_API void invert_camera(bool );

#endif // ORTHOCAMERACONTROLLER_H