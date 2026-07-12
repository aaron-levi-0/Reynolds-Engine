#ifndef GAMESTATE_H
#define GAMESTATE_H
#include <stdint.h>
#include <stdbool.h>
#include <cglm/cglm.h>

#include <utils/vector.h>

// forward declare to avoid heavy includes
struct Renderer;

typedef struct { vec2 pos, size; uint32_t texture; } SceneConfig;
typedef struct { int8_t value; uint8_t state; } Tile;   

typedef struct
{
    // --- game flow ---
    int scene;
    int game_state;
    bool win;
    bool lose;
    bool game_over_overlay;

    Tile* board;
    SceneConfig scenes[3];
    uint16_t num_bombs;
    float elapsed;              // seconds since the round started (status-bar timer)
} GameState;

typedef struct 
{
    Vector texture_register;

    vec2 sprite_cell_size;
    vec2 sprite_size;

     // --- texture id's --- 
    uint32_t menu_texture, options_texture;
    uint32_t easy_board, inter_board, hard_board;
    uint32_t board_pieces, button_restart, game_over, win_overlay;
    uint32_t face_happy, face_dead;     // status-bar face button
    
    // --- board config/state ---
    uint16_t width, height;
    uint16_t grid_width, grid_height;
    uint8_t tiles_x, tiles_y;

    // --- current scene layout ---
    vec2 board_pos;
    vec2 board_size;
    uint32_t board_type;
    vec3 bg_colour;

} DisplayContext;

#endif //GAMESTATE_H