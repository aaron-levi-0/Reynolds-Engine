#include "../include/timestep.h"

#include <stdbool.h>
#include <GLFW/glfw3.h>

typedef struct {
    float time;
    float last_frame_time;
    float capped_frame_time;
    bool is_capped;
} Timestep;

Timestep ts = {0};

float get_time()
{
    ts.time = (float)glfwGetTime();

    if(ts.is_capped && ts.time < ts.capped_frame_time)
       return ts.capped_frame_time;
    
    return ts.time;
}

float get_delta_time()
{
    float current_time = get_time();
    float delta_time = current_time - ts.last_frame_time;
    ts.last_frame_time = current_time;
    return delta_time;

}

void cap_frame_rate(float frame_rate)
{
    ts.capped_frame_time = 1.0f / frame_rate;
    ts.is_capped = true;
}