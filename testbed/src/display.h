#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <renderer/renderer.h>

#include "gamestate.h"
/* Manages the display of the game to the user. */

//functions
extern void init_textures();
extern void init_scenes();
extern void scene_controller(GameState* , struct Renderer* );
extern void calc_norms(GameState* , float* [] );
#endif