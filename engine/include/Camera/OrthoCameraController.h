#ifndef ORTHOCAMERACONTROLLER_H
#define ORTHOCAMERACONTROLLER_H

#include "layers.h"

typedef struct OrthographicCameraController OrthographicCameraController;

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