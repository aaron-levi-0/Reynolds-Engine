#ifndef EVENT_H
#define EVENT_H

#include "event_include.h"

#include <stdbool.h>
#include "defines.h"

#define EVENT_CLASS_TYPE(type) static const char* type##Event_getName() { return #type; }

/* 								
#define EVENT_CLASS_CATEGORY(category) static int category##Event_getCategoryFlags() { return category; }
 */
 
 
// Events  are currently blocking, meaning when an event occurs it
// immediately gets dispatched and must be dealt with right then an there.
// For the future, a better strategy might be to buffer events in an event
// bus and process them during the "event" part of the update stage.

typedef enum 
{
	NoEvent = 0,
	WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
	AppTick, AppUpdate, AppRender,
	KeyPressed, KeyReleased,
	MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScroll
} EventType;

typedef enum
{
	None = 0,
	EventCategoryApplication 	= BIT(0),
	EventCategoryInput 			= BIT(1),
	EventCategoryKeyboard 		= BIT(2),
	EventCategoryMouse 			= BIT(3),
	EventCategoryMouseButton 	= BIT(4)
} EventCategory;

// Event base structure
struct Event {
    EventType type;
    int categoryFlags;
	int (*getCategoryFlags)(void);
    const char* (*getName)(void);
    const char* (*debug)(void);
	bool handled;
};

extern bool isInCategory(Event* , EventCategory );
extern void setCurrentEvent(Event* );

#endif // EVENT_H