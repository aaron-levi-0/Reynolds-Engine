#ifndef ORTHOCAMERACONTROLLERINTERNALS_H
#define ORTHOCAMERACONTROLLERINTERNALS_H

#include "../include/OrthoCameraController.h"
#include "../include/layers.h"
#include "renderer/OrthoCameraInternals.h"

struct OrthographicCameraController{
	float aspect_ratio;
	float zoom_level;
	OrthoCamera camera;
	
	bool translation;
	bool rotation;
	bool zoom;
	bool inverted;
	
	float camera_translation_speed;
	float camera_rotation_speed;
};

#endif // ORTHOCAMERACONTROLLERINTERNALS_H