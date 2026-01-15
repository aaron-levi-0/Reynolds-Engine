#include "gameplay_layer.h"

#include <entry.h>          // bool Minimised

#include "common.h"
#include "game_logic.h"     // check_win()
#include "display.h"        // init_textures(), init_scenes()
#include "app_input.h"      // MouseInput(), KeyInput()

/*
    Since your LayerStack stores Layers BY VALUE (copies),
    we can't store per-layer state inside the Layer struct itself.
    So we keep a module-static pointer to the GameState.
*/
static GameState* g_state = NULL;
static DisplayContext* context = NULL;
static struct Renderer* st_render = NULL;

static void gameplay_update(float dt)
{
    if (!g_state) return;

    /*
        Core game flow:
        - While playing: evaluate win condition
        - If win/lose becomes true: transition to GAME_OVER
    */

    setMousePos();
    MouseInput(context, g_state);
    KeyInput(g_state);
    scene_update(context, g_state);

    if (g_state -> game_state == PLAY)
    {
        g_state -> win = check_win(context, g_state);

        if (g_state -> win || g_state -> lose)
        {
            g_state -> game_state = GAME_OVER;
            g_state -> game_over_overlay = !g_state -> win;
        }
    }
}

static void gameplay_render()
{
   	scene_render(st_render, context, g_state);
}

Layer create_gameplay_layer(struct Renderer* r, DisplayContext* dc, GameState* state)
{
    st_render = r;
    g_state = state;
    context = dc;
    Layer gameplay_layer = {
        .name   = "Gameplay Layer",
        .id     = LAYER_GAMEPLAY,  
        .update = gameplay_update,
        .render = gameplay_render
    };

    return gameplay_layer;
}
