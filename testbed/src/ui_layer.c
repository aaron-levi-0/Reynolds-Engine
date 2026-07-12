#include "ui_layer.h"

#include <logging.h>
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
   	ui_scene_render(st_render, context, g_state); // menus, options, overlays

    /* ---- immediate-mode UI demo panel ---- */
    UIBeginFrame(g_ui, g_dt);

    UIBeginPanel(g_ui, "Reynolds UI", (vec2){20.0f, 20.0f}, (vec2){280.0f, 250.0f});

    UILabel(g_ui, "It is alive.");

    if (UIButton(g_ui, "Log a message"))
        REYNOLDS_INFO("@ui demo: button clicked!");
    UITooltip(g_ui, "Writes a line to the console");

    UICheckbox(g_ui, "A checkbox", &demo_checkbox);
    UITooltip(g_ui, "Stored in a plain bool in ui_layer.c");

    UIBeginScroll(g_ui, "demo_scroll", 90.0f, &demo_scroll);
    UILabel(g_ui, "Scroll item 1");
    UILabel(g_ui, "Scroll item 2");
    UILabel(g_ui, "Scroll item 3");
    UILabel(g_ui, "Scroll item 4");
    UILabel(g_ui, "Scroll item 5");
    UILabel(g_ui, "Scroll item 6");
    UIEndScroll(g_ui);

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
