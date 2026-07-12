#ifndef UI_H
#define UI_H

#include <stdbool.h>
#include <cglm/cglm.h>

#include "defines.h"
#include "event_include.h"

/*
	Immediate-mode UI. No widget tree is stored: the game DECLARES the UI every
	frame between UIBeginFrame and UIEndFrame, and each widget call does layout,
	interaction and drawing on the spot, returning the result directly:

		UIBeginFrame(ui, dt);
		UIBeginPanel(ui, "Province", (vec2){20, 20}, (vec2){260, 200});
		UILabel(ui, "Flanders");
		if (UIButton(ui, "Build Temple"))
			start_construction(...);
		UITooltip(ui, "Costs 50 gold");
		UIEndPanel(ui);
		UIEndFrame(ui);

	COORDINATES: pixels, origin at the window's BOTTOM-LEFT, y up — the same
	handedness as the world (so DrawText works unchanged) and the same space
	glScissor uses. Panel positions are the panel's bottom-left corner.

	FRAME INTEGRATION: UIBeginFrame flushes the world batch and switches the
	renderer's view-projection to a pixel ortho matrix, so call the UI code
	from a layer that renders AFTER the world layers (render order is
	bottom -> top; push the UI layer above gameplay). EngineRun's final flush
	draws the UI batch. EngineRun resets the VP to the camera next frame.
*/

struct UIContext;
struct Renderer;
struct Font;

typedef struct
{
	vec4  panel_bg, title_bg;
	vec4  widget_bg, widget_hot, widget_active;
	vec4  text_colour, tooltip_bg, accent;
	float text_size;			// px
	float row_h;				// px height of one widget row
	float pad;					// px inner padding
	float tooltip_delay;		// seconds hovering before the tooltip shows
} UIStyle;

typedef enum
{
    UI_ALIGN_LEFT,
    UI_ALIGN_CENTER,
    UI_ALIGN_RIGHT
} UIAlignment;

REN_API struct UIContext* UICreate(struct Renderer* renderer, struct Font* font);
REN_API void UIDestroy(struct UIContext* ui);
REN_API UIStyle* UIGetStyle(struct UIContext* ui);		// tweak colours/sizes in place

/* Feed events (scroll wheel; click consumption over panels). Call from the UI
   layer's onEvent. Marks mouse events handled when the cursor is over the UI. */
REN_API void UIOnEvent(struct UIContext* ui, Event* e);

/* True while the cursor is over a panel or a widget is being pressed —
   layers that POLL input (ADR-0006) should skip mouse handling when set. */
REN_API bool UIWantsMouse(struct UIContext* ui);

REN_API void UIBeginFrame(struct UIContext* ui, float dt);
REN_API void UIEndFrame(struct UIContext* ui);
REN_API void UINextWidth(struct UIContext* ui, float w);	// next widget's width (0 = default row width)
REN_API void UISameLine(struct UIContext* ui);

/* position = bottom-left corner, size = width/height, both in pixels. */
REN_API void UIBeginPanel(struct UIContext* ui, const char* title, const vec2 position, const vec2 size);
REN_API void UIEndPanel(struct UIContext* ui);

REN_API void UILabel(struct UIContext* ui, const char* text, UIAlignment alignment);
REN_API bool UIButton(struct UIContext* ui, const char* label);		// true on click (release over the button)
REN_API bool UICheckbox(struct UIContext* ui, const char* label, bool* value);
REN_API bool UIImageButton(struct UIContext* ui, const char* id_str, uint32_t textureID, const vec4 tex_coords, float size);
REN_API void UITooltip(struct UIContext* ui, const char* text);		// attaches to the PREVIOUS widget

/* Scrollable region inside a panel. `scroll` is caller-owned state (px,
   0 = top). Content between Begin/End is clipped; the wheel scrolls it
   while the cursor is inside. EXPERIMENTAL — see progress.md. */
REN_API void UIBeginScroll(struct UIContext* ui, const char* id, float view_height, float* scroll);
REN_API void UIEndScroll(struct UIContext* ui);

#endif // UI_H
