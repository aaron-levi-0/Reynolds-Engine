#include "app_event.h"

static WindowEvent windowEvent;
static Event* event = &windowEvent.event;

static char buffer[256];

// FUNCTION IMPLEMENTATIONS //

int WindowEvent_getCategoryFlags() { return EventCategoryApplication; }

// WindowResizeEvent

EVENT_CLASS_TYPE(WindowResize)

static const char* WindowResizeEvent_debug()
{
    snprintf(buffer, sizeof(buffer), "@window: width: %d , height: %d", windowEvent.width, windowEvent.height);
    return buffer;
}

Event* createWindowResizeEvent (unsigned int width, unsigned int height)
{	
    event -> type 				= WindowResize;
    event -> categoryFlags 		= EventCategoryApplication;
    event -> getName 			= WindowResizeEvent_getName;
    event -> getCategoryFlags 	= WindowEvent_getCategoryFlags;
    event -> debug	 			= WindowResizeEvent_debug;
    event -> handled			= false;
	
	windowEvent.width	= width;
	windowEvent.height 	= height;
	
	setCurrentEvent(event);
	return event;
}

// WindowMovedEvent

EVENT_CLASS_TYPE(WindowMoved)

static const char* WindowMovedEvent_debug()
{
    snprintf(buffer, sizeof(buffer), "@window: moved window to: (%d, %d)", windowEvent.xpos, windowEvent.ypos);
    return buffer;
}

Event* createWindowMovedEvent(int xpos, int ypos)
{	
    event -> type 				= WindowMoved;
    event -> categoryFlags 		= EventCategoryApplication;
    event -> getName 			= WindowMovedEvent_getName;
    event -> getCategoryFlags 	= WindowEvent_getCategoryFlags;
    event -> debug	 			= WindowMovedEvent_debug;
    event -> handled			= false;
	
	windowEvent.xpos	= xpos;
	windowEvent.ypos 	= ypos;
	
	setCurrentEvent(event);
	return event;
}

// WindowCloseEvent

EVENT_CLASS_TYPE(WindowClose)

static const char* WindowCloseEvent_debug()
{
    snprintf(buffer, sizeof(buffer), "@window: closing window...");
    return buffer;
}

Event* createWindowCloseEvent()
{	
    event -> type 				= WindowClose;
    event -> categoryFlags 		= EventCategoryApplication;
    event -> getName 			= WindowCloseEvent_getName;
    event -> getCategoryFlags 	= WindowEvent_getCategoryFlags;
    event -> debug	 			= WindowCloseEvent_debug;
    event -> handled			= false;
	
	setCurrentEvent(event);
	return event;
}
