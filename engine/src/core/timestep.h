#ifndef TIMESTEP_H
#define TIMESTEP_H

#include "glx.h"

REN_API float get_time();
REN_API float get_delta_time();
REN_API void cap_frame_rate(float );
#endif