#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H

#include "defines.h"

REN_API void createOrthoCamera(float left, float right, float bottom, float top);
REN_API void* getPVMat();

#endif