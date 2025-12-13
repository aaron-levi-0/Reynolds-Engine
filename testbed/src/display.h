#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <renderer/renderer.h>

/* Manages the display of the game to the user. */

//variables
extern bool game_over_overlay;
extern int 	game_state;
extern int 	scene;
extern int 	board_disp;
extern bool	init_state;

//functions
extern void init_textures();
extern void scene_controller(struct Renderer* );

extern float* getConfigPosition();
extern float* getConfigSize();

#endif