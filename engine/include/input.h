#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include "defines.h"

REN_API bool isKeyPressed(int );
REN_API bool isMouseButtonPressed(int );

REN_API void setMousePos();
REN_API void getMousePos(double* x, double* y);

#endif // INPUT_H