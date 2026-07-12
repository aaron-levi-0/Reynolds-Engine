#include "ui_layer.h"

#include <logging.h>
#include "display.h" 
#include "common.h"

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
static struct UIContext* g_ui = NULL;
static float g_dt = 0.0f;

static bool  demo_checkbox = false;
static float demo_scroll   = 0.0f;

static void ui_update(float dt)
{
    g_dt = dt;

    if (!g_state) return;
    ui_scene_update(context, g_state); // menu-scene logic (e.g. camera reset)
}

static void ui_event(Event* e)
{
    UIOnEvent(g_ui, e);                // wheel capture + eats clicks over panels
}

static void ui_render()
{
    char mines[8], time_str[8];
    
   	ui_scene_render(st_render, context, g_state); // menus, options, overlays

    /* ---- immediate-mode UI demo panel ---- */
    UIBeginFrame(g_ui, g_dt);
    
    //snprintf(mines, sizeof mines, "%03d", mines_left);
   // snprintf(time_str, sizeof time_str, "%03d", seconds);

    vec2 bar_pos = {20, getWindowHeight() - 64};

    UIBeginPanel(g_ui, "##statusbar", bar_pos, (vec2){bar_w, 44.0f});

    UINextWidth(g_ui, 60.0f);  UILabel(g_ui, mines);
    UISameLine(g_ui);
    UINextWidth(g_ui, 32.0f);
    if (UIImageButton(g_ui, "smiley", context -> texture, smiley_uv, 32.0f))
        reset_game(context, g_state);
    UISameLine(g_ui);
    UINextWidth(g_ui, 60.0f);  UILabel(g_ui, time_str);

    UIEndPanel(g_ui);
    UIEndFrame(g_ui);
}

Layer create_ui_layer(struct Renderer* r, DisplayContext* dc, GameState* state, struct UIContext* ui)
{
    st_render = r;
    g_state = state;
    context = dc;
    g_ui = ui;
    Layer ui_layer = {
        .name   = "UI Layer",
        .id     = LAYER_UI,  
        .update = ui_update,
        .render = ui_render,
        .onEvent = ui_event
    };

    return ui_layer;
}
