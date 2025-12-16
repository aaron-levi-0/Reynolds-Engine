#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <stdint.h>
#include <stdbool.h>
#include <cglm/cglm.h>

// forward declare to avoid heavy includes
struct Renderer;

typedef struct
{
    // --- game flow ---
    int scene;
    int game_state;
    bool win;
    bool lose;
    bool game_over_overlay;

    // --- board config/state ---
    uint16_t width, height;
    uint16_t grid_width, grid_height;
    uint8_t tiles_x, tiles_y;
    uint16_t num_bombs;

    int** board;            // row pointers
    int** reveal_state;

    // current scene layout
    vec2 config_pos;
    vec2 config_size;
    uint32_t config_texture;

    vec3 bg_colour;

} GameState;

#endif //GAMESTATE_H