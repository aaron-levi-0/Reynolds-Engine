#include "ui_layer.h"

#include <stdio.h>
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

/* the classic status bar: [mines left] [face button] [timer] */
static void status_bar()
{
   /* mines left = bombs - flags placed; the board is small, count each frame */
    int flags = 0;
    int tiles = (int)context -> tiles_x * (int)context -> tiles_y;
    for (int i = 0; i < tiles; i++)
        if (g_state -> board[i].state == IS_TILE_FLAGGED)
            flags++;

    char mines[8], time_str[8];

    snprintf(mines,    sizeof mines,    "%03d", (int)g_state -> num_bombs - flags);
    snprintf(time_str, sizeof time_str, "%03d", (int)g_state -> elapsed);

    float window_width = (float)getWindowWidth();
    float window_height = (float)getWindowHeight();
    float bar_height = window_height * 0.125f;

    vec2 bar_pos = {0, window_height - bar_height};

    UIStyle* style = UIGetStyle(g_ui);

    style -> panel_bg[3] = 1.0f;

    style->text_size = bar_height * 0.42f;
    style->row_h = bar_height - style->pad * 2.0f;

    float content_width = window_width - style->pad * 2.0f;
    float column_width = (content_width - style->pad * 2.0f) / 3.0f;

    UIBeginPanel(g_ui, "##statusbar", bar_pos, (vec2){window_width, bar_height});
    
    UINextWidth(g_ui, column_width);
    UILabel(g_ui, mines, UI_ALIGN_CENTER);
    UISameLine(g_ui);
    
    /* immediate mode: the face is re-chosen from game state every frame */
    uint32_t face = g_state -> lose ? context -> face_dead : context -> face_happy;

    if (UIImageButton(g_ui, "face", face, (vec4){0.0f, 0.0f, 1.0f, 1.0f}, bar_height * 0.9f))
        g_state -> game_state = RESTART;        // display.c's scene_update handles the reset

    UINextWidth(g_ui, column_width);

    UISameLine(g_ui);
    UINextWidth(g_ui, column_width);
    UILabel(g_ui, time_str, UI_ALIGN_CENTER);

    UIEndPanel(g_ui);
}

static void ui_render()
{
   	ui_scene_render(st_render, context, g_state); // menus, options, overlays
    UIBeginFrame(g_ui, g_dt);                  // flush world, switch to screen space

    if (g_state -> board && (g_state -> scene & ALL_BOARDS))
        status_bar();                          // only while a board is on screen

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
