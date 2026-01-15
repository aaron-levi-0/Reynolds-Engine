#ifndef ORTHOCAMERAINTERNALS_H
#define ORTHOCAMERAINTERNALS_H

#include "../include/OrthographicCamera.h"
#include <cglm/cglm.h>

typedef struct {
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;
	mat4 PVMatrix;
	
	vec3 position;
	float rotation;	//about z-axis only
} OrthoCamera;

extern void setOrthoProjection(float left, float right, float bottom, float top); 
extern void setOrthoPosition(float* , bool );
extern void setOrthoRotation(float , bool );

extern void* getOrthoPosition();
extern float getOrthoRotation();
extern void* getOrthoProjection();
extern void* getOrthoView();

#endif