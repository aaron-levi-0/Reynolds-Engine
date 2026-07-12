#include "ui.h"

#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "renderer.h"
#include "text.h"
#include "input.h"
#include "window.h"
#include "keycodes.h"

/*
	The whole immediate-mode machine is two uint32 IDs:

	  hot    = the widget the cursor is over this frame (last one wins — widgets
	           submitted later draw on top, so they should win the hover too)
	  active = the widget that was under the cursor when the button went down;
	           it stays active while the button is held, even if the cursor
	           slides off (that is what makes press-drag-release feel right)

	A "click" is: button RELEASED over a widget that is both hot and active.
	Widgets are identified by hashing their label, seeded by the panel title,
	so IDs are stable across frames without storing anything.
	Limitation: two widgets with the same label in the same panel share an ID.
*/

struct UIContext
{
	struct Renderer* r;
	struct Font*     font;
	UIStyle          style;

	/* input snapshot (UI space: px, origin bottom-left, y up) */
	float mouse_x, mouse_y;
	bool  down, prev_down;
	bool  clicked, released;		// edges this frame
	float scroll_delta;				// wheel movement this frame (+1 = up)
	float pending_scroll;			// accumulated by UIOnEvent between frames
	float dt;

	/* interaction state that survives across frames */
	uint32_t hot, prev_hot, active;
	float    hot_time;
	bool     over_ui, prev_over_ui;

	/* layout pen: cursor_y is the TOP of the next row (rows flow downward) */
	float    cursor_x, cursor_y, row_w;
	float    panel_left;			// content left edge; the pen returns here each row
	uint32_t panel_seed;
	bool     in_panel;

	/* clip / scroll region */
	bool   clip_on;
	float  clip_x, clip_y, clip_w, clip_h;
	float* scroll_val;
	float  scroll_top;				// cursor_y when the region began (content top)

	/* tooltip (drawn last, in UIEndFrame, so it sits on top of everything) */
	uint32_t    last_id;
	const char* tooltip_text;

	float last_x, last_y, last_w, last_h;    // rect of the widget just submitted
	float next_w;                             // 0 = default (full row width)
};

/* ---------------- helpers ---------------- */

static uint32_t hash_id(uint32_t seed, const char* s)
{
	uint32_t h = seed ? seed : 2166136261u;			// FNV-1a
	while (*s)
	{
		h ^= (uint8_t)*s++;
		h *= 16777619u;
	}
	return h ? h : 1;								// 0 means "no widget"
}

static bool rect_contains(float x, float y, float w, float h, float px, float py)
{
	return px >= x && px <= x + w && py >= y && py <= y + h;
}

static bool mouse_in(struct UIContext* ui, float x, float y, float w, float h)
{
	bool inside = rect_contains(x, y, w, h, ui -> mouse_x, ui -> mouse_y);

	if (ui -> clip_on)		// a clipped widget only reacts where it is visible
		inside = inside && rect_contains(ui -> clip_x, ui -> clip_y,
		                                 ui -> clip_w, ui -> clip_h,
		                                 ui -> mouse_x, ui -> mouse_y);
	return inside;
}

/* layout + interaction shared by every widget; returns true on click */
static bool widget_behaviour(struct UIContext* ui, uint32_t id, float x, float y, float w, float h)
{
	bool inside = mouse_in(ui, x, y, w, h);

	if (inside)
	{
		ui -> hot = id;
		if (ui -> clicked)
			ui -> active = id;
	}

	ui -> last_id = id;
	return inside && ui -> released && ui -> active == id;
}

/* allocate the next widget's rect from the pen; consumes UINextWidth (one-shot),
   records the rect for UISameLine, and resets the pen to the panel's left edge */
static void next_widget(struct UIContext* ui, float h, float* x, float* y, float* w)
{
	/* default width = whatever remains of the row (shrinks after UISameLine) */
	float remaining = (ui -> panel_left + ui -> row_w) - ui -> cursor_x;
	*w = ui -> next_w > 0.0f ? ui -> next_w : remaining;
	ui -> next_w = 0.0f;

	*x = ui -> cursor_x;
	*y = ui -> cursor_y - h;

	ui -> last_x = *x;
	ui -> last_y = *y;
	ui -> last_w = *w;
	ui -> last_h = h;

	ui -> cursor_x = ui -> panel_left;			// a normal row starts at the left edge
	ui -> cursor_y = *y - ui -> style.pad * 0.5f;
}

void UISameLine(struct UIContext* ui)
{
    ui -> cursor_x = ui -> last_x + ui -> last_w + ui -> style.pad;
	ui -> cursor_y = ui -> last_y + ui -> last_h;	// back up to the previous row's top
}

void UINextWidth(struct UIContext* ui, float w) { ui -> next_w = w; }

static float baseline_in(struct UIContext* ui, float row_bottom, float row_h)
{
	/* rough vertical centring for the baked font: baseline sits a bit above
	   the row bottom to leave space for descenders. Tune via style if needed. */
	return row_bottom + (row_h - ui -> style.text_size) * 0.5f + ui -> style.text_size * 0.22f;
}

/* ---------------- lifecycle ---------------- */

struct UIContext* UICreate(struct Renderer* renderer, struct Font* font)
{
	ASSERT_FATAL(renderer, "@ui: UICreate given NULL renderer.");
	if (!font)
		REYNOLDS_WARN("@ui: UICreate given NULL font — labels will not draw.");

	struct UIContext* ui = calloc(1, sizeof(struct UIContext));
	ASSERT_FATAL(ui, "@ui: failed to allocate UIContext.");

	ui -> r    = renderer;
	ui -> font = font;

	ui -> style = (UIStyle){
		.panel_bg      = {0.13f, 0.14f, 0.17f, 0.95f},
		.title_bg      = {0.20f, 0.22f, 0.28f, 1.0f},
		.widget_bg     = {0.25f, 0.27f, 0.33f, 1.0f},
		.widget_hot    = {0.33f, 0.36f, 0.44f, 1.0f},
		.widget_active = {0.16f, 0.18f, 0.22f, 1.0f},
		.text_colour   = {0.92f, 0.93f, 0.95f, 1.0f},
		.tooltip_bg    = {0.08f, 0.08f, 0.10f, 0.97f},
		.accent        = {0.55f, 0.75f, 0.95f, 1.0f},
		.text_size     = 18.0f,
		.row_h         = 26.0f,
		.pad           = 8.0f,
		.tooltip_delay = 0.4f,
	};

	return ui;
}

void UIDestroy(struct UIContext* ui)
{
	free(ui);
}

UIStyle* UIGetStyle(struct UIContext* ui)
{
	return &ui -> style;
}

void UIOnEvent(struct UIContext* ui, Event* e)
{
	if (!ui) return;

	if (e -> type == MouseScroll)
	{
		ui -> pending_scroll += (float)e -> mouse.y;
		if (ui -> prev_over_ui)
			e -> handled = true;
	}
	else if (e -> type == MouseButtonPressed || e -> type == MouseButtonReleased)
	{
		/* the UI eats clicks that land on a panel so event-driven layers
		   below never see them (ADR-0002). Uses last frame's panel rects —
		   one frame of latency, invisible in practice. */
		if (ui -> prev_over_ui)
			e -> handled = true;
	}
}

bool UIWantsMouse(struct UIContext* ui)
{
	return ui && (ui -> prev_over_ui || ui -> active != 0);
}

void UIBeginFrame(struct UIContext* ui, float dt)
{
	/* everything the world drew so far goes to the GPU with the camera VP;
	   from here on quads are in screen space */
	EndBatch(ui -> r);
	FlushBatch(ui -> r);
	BeginBatch(ui -> r);

	float w = (float)getWindowWidth();
	float h = (float)getWindowHeight();

	mat4 screen;
	glm_ortho(0.0f, w, 0.0f, h, -1.0f, 1.0f, screen);
	setViewProjection(ui -> r, screen);

	/* input snapshot: one flip, here, and the rest of the module lives in
	   the same y-up space as the world and glScissor */
	double mx, my;
	getMousePos(&mx, &my);
	ui -> mouse_x = (float)mx;
	ui -> mouse_y = h - (float)my;

	ui -> prev_down = ui -> down;
	ui -> down      = isMouseButtonPressed(MOUSE_BUTTON_1);
	ui -> clicked   = ui -> down && !ui -> prev_down;
	ui -> released  = !ui -> down && ui -> prev_down;

	ui -> scroll_delta   = ui -> pending_scroll;
	ui -> pending_scroll = 0.0f;
	ui -> dt = dt;

	ui -> prev_hot     = ui -> hot;
	ui -> hot          = 0;
	ui -> prev_over_ui = ui -> over_ui;
	ui -> over_ui      = false;

	ui -> tooltip_text = NULL;
	ui -> last_id      = 0;
	ui -> in_panel     = false;
	ui -> clip_on      = false;
}

void UIEndFrame(struct UIContext* ui)
{
	if (ui -> in_panel)
		REYNOLDS_WARN("@ui: UIEndFrame with an open panel — missing UIEndPanel?");

	/* hover timing drives the tooltip delay */
	if (ui -> hot != 0 && ui -> hot == ui -> prev_hot)
		ui -> hot_time += ui -> dt;
	else
		ui -> hot_time = 0.0f;

	if (ui -> released)
		ui -> active = 0;

	/* deferred tooltip — drawn after every widget, so it is always on top */
	if (ui -> tooltip_text && ui -> hot_time >= ui -> style.tooltip_delay)
	{
		float ts  = ui -> style.text_size * 0.9f;
		float tw  = TextWidth(ui -> font, ui -> tooltip_text, ts);
		float pad = ui -> style.pad;
		float bw  = tw + pad * 2.0f;
		float bh  = ts + pad * 1.5f;
		float bx  = ui -> mouse_x + 14.0f;
		float by  = ui -> mouse_y - bh - 6.0f;

		/* keep it on screen */
		float sw = (float)getWindowWidth();
		if (bx + bw > sw) bx = sw - bw;
		if (by < 0.0f)    by = ui -> mouse_y + 18.0f;

		DrawColour(ui -> r, (vec2){bx, by}, (vec2){bw, bh}, ui -> style.tooltip_bg);
		DrawText(ui -> r, ui -> font, ui -> tooltip_text,
		         (vec2){bx + pad, by + pad * 0.75f + ts * 0.15f}, ts, ui -> style.text_colour);
	}
}

/* ---------------- containers ---------------- */

void UIBeginPanel(struct UIContext* ui, const char* title, const vec2 position, const vec2 size)
{
	if (ui -> in_panel)
	{
		REYNOLDS_WARN("@ui: UIBeginPanel inside another panel — call UIEndPanel first.");
		return;
	}
	bool decorated = !(title[0] == '#' && title[1] == '#');

	ui -> in_panel   = true;
	ui -> panel_seed = hash_id(0, title);

	if (rect_contains(position[0], position[1], size[0], size[1], ui -> mouse_x, ui -> mouse_y))
		ui -> over_ui = true;

	float title_h = ui -> style.row_h;

	DrawColour(ui -> r, position, size, ui -> style.panel_bg);
	
	if (decorated)
	{
		DrawColour(ui -> r, (vec2){position[0], position[1] + size[1] - title_h},
				(vec2){size[0], title_h}, ui -> style.title_bg);
		DrawText(ui -> r, ui -> font, title,
				(vec2){position[0] + ui -> style.pad,
						baseline_in(ui, position[1] + size[1] - title_h, title_h)},
				ui -> style.text_size, ui -> style.text_colour);
	}

	/* the pen starts under the title bar; rows flow downward */
	/* the pen starts under the title bar (undecorated panels have none);
	   rows flow downward */
	ui -> panel_left = position[0] + ui -> style.pad;
	ui -> cursor_x   = ui -> panel_left;
	ui -> cursor_y   = position[1] + size[1] - (decorated ? title_h : 0.0f) - ui -> style.pad;
	ui -> row_w      = size[0] - ui -> style.pad * 2.0f;
}

void UIEndPanel(struct UIContext* ui)
{
	ui -> in_panel = false;
}

/* ---------------- widgets ---------------- */

void UILabel( struct UIContext* ui, const char* text, UIAlignment alignment)
{
    uint32_t id = hash_id(ui->panel_seed, text);
    float h = ui->style.row_h;
    float x, y, w;

    next_widget(ui, h, &x, &y, &w);
    widget_behaviour(ui, id, x, y, w, h);

    float text_width = TextWidth(
        ui->font,
        text,
        ui->style.text_size
    );

    float text_x = x;

    if (alignment == UI_ALIGN_CENTER)
        text_x += (w - text_width) * 0.5f;
    else if (alignment == UI_ALIGN_RIGHT)
        text_x += w - text_width;

    DrawText(
        ui->r,
        ui->font,
        text,
        (vec2){text_x, baseline_in(ui, y, h)},
        ui->style.text_size,
        ui->style.text_colour
    );
}

bool UIButton(struct UIContext* ui, const char* label)
{
	uint32_t id = hash_id(ui -> panel_seed, label);
	float h = ui -> style.row_h;
	float x, y, w;
	next_widget(ui, h, &x, &y, &w);

	bool click = widget_behaviour(ui, id, x, y, w, h);

	vec4* bg = &ui -> style.widget_bg;
	if (ui -> active == id && ui -> down)	bg = &ui -> style.widget_active;
	else if (ui -> hot == id)				bg = &ui -> style.widget_hot;

	DrawColour(ui -> r, (vec2){x, y}, (vec2){w, h}, *bg);

	float tw = TextWidth(ui -> font, label, ui -> style.text_size);
	DrawText(ui -> r, ui -> font, label,
	         (vec2){x + (w - tw) * 0.5f, baseline_in(ui, y, h)},
	         ui -> style.text_size, ui -> style.text_colour);

	return click;
}

bool UIImageButton(struct UIContext* ui, const char* id_str, uint32_t textureID,
                   const vec4 tex_coords, float size)
{
    uint32_t id = hash_id(ui -> panel_seed, id_str);
	float x, y, w;
	UINextWidth(ui, size);					// an image button is always square
	next_widget(ui, size, &x, &y, &w);

	bool click = widget_behaviour(ui, id, x, y, size, size);

    vec4* bg = &ui -> style.widget_bg;
	if (ui -> active == id && ui -> down)	bg = &ui -> style.widget_active;
	else if (ui -> hot == id)				bg = &ui -> style.widget_hot;

	DrawColour(ui -> r, (vec2){x - 2, y - 2}, (vec2){size + 4, size + 4}, *bg);	// bevel/hover ring
	DrawQuad(ui -> r, (vec2){x, y}, (vec2){size, size}, textureID, tex_coords);

    return click;
}

bool UICheckbox(struct UIContext* ui, const char* label, bool* value)
{
	uint32_t id = hash_id(ui -> panel_seed, label);
	float h = ui -> style.row_h;
	float x, y, w;
	next_widget(ui, h, &x, &y, &w);

	bool click = widget_behaviour(ui, id, x, y, w, h);	
	if (click && value)
		*value = !*value;

	float box = h * 0.65f;
	float by  = y + (h - box) * 0.5f;

	DrawColour(ui -> r, (vec2){x, by}, (vec2){box, box},
	           ui -> hot == id ? ui -> style.widget_hot : ui -> style.widget_bg);

	if (value && *value)
	{
		float in = box * 0.25f;
		DrawColour(ui -> r, (vec2){x + in, by + in},
		           (vec2){box - in * 2.0f, box - in * 2.0f}, ui -> style.accent);
	}

	DrawText(ui -> r, ui -> font, label,
	         (vec2){x + box + ui -> style.pad, baseline_in(ui, y, h)},
	         ui -> style.text_size, ui -> style.text_colour);

	return click;
}

void UITooltip(struct UIContext* ui, const char* text)
{
	/* register only if the widget just submitted is the hovered one */
	if (ui -> last_id != 0 && ui -> last_id == ui -> hot)
		ui -> tooltip_text = text;
}

/* ---------------- scroll region (experimental) ---------------- */

void UIBeginScroll(struct UIContext* ui, const char* id, float view_height, float* scroll)
{
	(void)id;

	ui -> scroll_val = scroll;
	ui -> clip_x = ui -> cursor_x;
	ui -> clip_y = ui -> cursor_y - view_height;
	ui -> clip_w = ui -> row_w;
	ui -> clip_h = view_height;
	ui -> clip_on = true;

	SetClipRect(ui -> r, (vec2){ui -> clip_x, ui -> clip_y}, (vec2){ui -> clip_w, ui -> clip_h});

	/* shifting the pen UP by the scroll amount slides content up out of the
	   top of the window (which the clip hides) and reveals the rest below */
	ui -> scroll_top = ui -> cursor_y;
	ui -> cursor_y  += *scroll;
}

void UIEndScroll(struct UIContext* ui)
{
	float content_h  = (ui -> scroll_top + *ui -> scroll_val) - ui -> cursor_y;
	float max_scroll = content_h - ui -> clip_h;
	if (max_scroll < 0.0f) max_scroll = 0.0f;

	if (rect_contains(ui -> clip_x, ui -> clip_y, ui -> clip_w, ui -> clip_h,
	                  ui -> mouse_x, ui -> mouse_y))
		*ui -> scroll_val -= ui -> scroll_delta * ui -> style.row_h;

	if (*ui -> scroll_val < 0.0f)        *ui -> scroll_val = 0.0f;
	if (*ui -> scroll_val > max_scroll)  *ui -> scroll_val = max_scroll;

	ClearClipRect(ui -> r);
	ui -> clip_on = false;

	/* proportional scrollbar on the right edge */
	if (max_scroll > 0.0f)
	{
		float track_x = ui -> clip_x + ui -> clip_w - 4.0f;
		float frac    = ui -> clip_h / content_h;
		float th      = ui -> clip_h * frac;
		float ty      = ui -> clip_y + (ui -> clip_h - th)
		                * (1.0f - *ui -> scroll_val / max_scroll);

		DrawColour(ui -> r, (vec2){track_x, ui -> clip_y}, (vec2){4.0f, ui -> clip_h},
		           ui -> style.widget_active);
		DrawColour(ui -> r, (vec2){track_x, ty}, (vec2){4.0f, th}, ui -> style.widget_hot);
	}

	/* the pen continues below the region */
	ui -> cursor_y = ui -> clip_y - ui -> style.pad * 0.5f;
}
