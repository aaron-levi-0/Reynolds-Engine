#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include "defines.h"

REN_API bool isKeyPressed(int );
REN_API bool isMouseButtonPressed(int );

REN_API void setMousePos();

#endif // INPUT_H