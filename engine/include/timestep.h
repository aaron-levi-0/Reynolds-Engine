#ifndef TIMESTEP_H
#define TIMESTEP_H

#include "defines.h"

REN_API float get_time();
REN_API float get_delta_time();
REN_API void cap_frame_rate(float );

#endif // TIMESTEP_H