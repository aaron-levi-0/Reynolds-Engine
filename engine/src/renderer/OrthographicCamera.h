#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H
#include "core/glx.h"

typedef struct {
	
	mat4 ProjectionMatrix;
	mat4 ViewMatrix;
	mat4 PVMatrix;
	
	vec3 position;
	float rotation;	//about z-axis only
} OrthoCamera;

extern void createOrthoCamera(float left, float right, float bottom, float top);

extern void setOrthoProjection(float left, float right, float bottom, float top); 
extern void setOrthoPosition(float* , bool );
extern void setOrthoRotation(float , bool );

extern void* getOrthoPosition();
extern float getOrthoRotation();
extern void* getOrthoProjection();
extern void* getOrthoView();
extern void* getPVMat();

#endif