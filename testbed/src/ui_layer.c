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

static void ui_update(float dt)
{
    if (!g_state) return;

    setMousePos();
    MouseInput(context, g_state);
    KeyInput(g_state);
    ui_scene_update(context, g_state);
}

static void ui_render()
{
   	ui_scene_render(st_render, context, g_state);
}

Layer create_ui_layer(struct Renderer* r, DisplayContext* dc, GameState* state)
{
    st_render = r;
    g_state = state;
    context = dc;
    Layer ui_layer = {
        .name   = "UI Layer",
        .id     = LAYER_UI,  
        .update = ui_update,
        .render = ui_render
    };

    return ui_layer;
}
