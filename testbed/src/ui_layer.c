#include "ui_layer.h"

#include "display.h" 

/*
    The LayerStack holds layers by value, so per-instance state can't live in
    the Layer struct — we keep module-static pointers instead (ADR-0003).

    Responsibility: menus and overlays only. Input is polled once, in the
    gameplay layer (ADR-0006), so this layer no longer touches input — that
    removes the duplicate per-frame input handling the two layers used to do.
*/

static GameState* g_state = NULL;
static DisplayContext* context = NULL;
static struct Renderer* st_render = NULL;

static void ui_update(float dt)
{
    (void)dt;

    if (!g_state) return;
    ui_scene_update(context, g_state); // menu-scene logic (e.g. camera reset)
}

static void ui_render()
{
   	ui_scene_render(st_render, context, g_state); // menus, options, overlays
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
