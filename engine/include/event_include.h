#ifndef EVENT_INCLUDE_H
#define EVENT_INCLUDE_H

#include <stdbool.h>
#include <stdint.h>

#include "defines.h"

typedef enum {
	NoEvent = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
	AppTick, AppUpdate, AppRender,
	KeyPressed, KeyReleased,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScroll
} EventType;

typedef struct {
    EventType type;
    bool      handled;
    union {                                   // only the member for `type` is valid
        struct { int code, repeat; }        key;     // KEY_PRESSED / KEY_RELEASED
        struct { int button; double x, y; } button;  // MOUSE_PRESSED / MOUSE_RELEASED
        struct { double x, y; }             mouse;   // MOUSE_MOVED / MOUSE_SCROLLED
        struct { uint32_t width, height; }  resize;  // WINDOW_RESIZE
        struct { uint32_t x, y; }           moved;   // WINDOW_MOVED
    };
} Event;

REN_API double getMouseX(Event* event);
REN_API double getMouseY(Event* event);

#endif // EVENT_INCLUDE_H