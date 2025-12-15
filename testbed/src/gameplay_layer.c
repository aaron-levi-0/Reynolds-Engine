#include "gameplay_layer.h"

#include "game_logic.h"     // check_win(...)
#include "common.h"
#include "display.h"        // init_textures(), init_scenes()
#include "core/input.h"     // MouseInput(), KeyInput()

/*
    Since your LayerStack stores Layers BY VALUE (copies),
    we can't store per-layer state inside the Layer struct itself.
    So we keep a module-static pointer to the GameState.
*/
static GameState* g_state = NULL;

static void gameplay_update(float dt)
{
    if (!g_state) return;

    /*
        One-time init hook. You already have init_state in GameState.
        Use this for scene setup, board creation, etc.
        Right now we just flip it off so you have a reliable “run once” spot.
    */
    if (g_state -> init_state)
    {
        init_textures();
	    init_scenes();
        g_state -> init_state = false;
    }

    /*
        Core game flow:
        - While playing: evaluate win condition
        - If win/lose becomes true: transition to GAME_OVER
    */
    if (g_state -> game_state == PLAY)
    {
        g_state -> win = check_win(g_state);

        if (g_state -> win || g_state -> lose)
        {
            g_state -> game_state = GAME_OVER;
            g_state -> game_over_overlay = !g_state -> win;
        }
    }
}

static void gameplay_onEvent(Event* e)
{
    setMousePos();
    // MouseInput(&state);
    // KeyInput(&state);
}

Layer create_gameplay_layer(GameState* state)
{
    g_state = state;

    Layer gameplay_layer = {
        .name   = "Gameplay Layer",
        .id     = LAYER_GAMEPLAY,  
        .update = gameplay_update,
        .onEvent= gameplay_onEvent
    };

    return gameplay_layer;
}
