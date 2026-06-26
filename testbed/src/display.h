#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>

#include "gamestate.h"
/* Manages the display of the game to the user. */

//functions
extern void init_display(DisplayContext* dc);
extern void init_scenes(DisplayContext* dc, GameState* s);
extern void scene_update(DisplayContext* dc, GameState* s);
extern void scene_render(struct Renderer* r, DisplayContext* dc, GameState* s);
extern void ui_scene_update(DisplayContext* dc, GameState* s);
extern void ui_scene_render(struct Renderer* r, DisplayContext* dc, GameState* s);
extern void calc_norms(DisplayContext* dc, float* [] );

extern void unload_textures(DisplayContext* dc);
#endif