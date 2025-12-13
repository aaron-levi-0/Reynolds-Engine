#include "mouse_event.h"

MouseEvent mouseEvent;
static char buffer[256];

static Event* event = &mouseEvent.event;

// FUNCTION IMPLEMENTATIONS //

//EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
int MouseEvent_getCategoryFlags() { return EventCategoryMouse | EventCategoryInput; }

// MouseMovedEvent

EVENT_CLASS_TYPE(MouseMoved)

static const char* MouseMovedEvent_debug() 
{
    snprintf(buffer, sizeof(buffer), "@input: mouse moved: xpos: %.2f\typos: %.2f", mouseEvent.coordinates.mouseX, mouseEvent.coordinates.mouseY);
    return buffer;
}

Event* createMouseMovedEvent(double xpos, double ypos) 
{	
    event -> type 				= MouseMoved;
    event -> categoryFlags 		= EventCategoryMouse | EventCategoryInput;
    event -> getName 			= MouseMovedEvent_getName;
	event -> getCategoryFlags 	= MouseEvent_getCategoryFlags;
    event -> debug 				= MouseMovedEvent_debug;
    event -> handled			= false;
	
	mouseEvent.coordinates.mouseX = xpos;
	mouseEvent.coordinates.mouseY = ypos;
	
	setCurrentEvent(event);
	
	return event;
}

// MouseScrolledEvent

EVENT_CLASS_TYPE(MouseScroll)

static const char* MouseScrollEvent_debug() 
{
    snprintf(buffer, sizeof(buffer), "@input: mouse scrolled: xoffset: %.2f\tyoffset: %.2f", mouseEvent.coordinates.mouseX, mouseEvent.coordinates.mouseY);
    return buffer;
}

Event* createMouseScrollEvent(double xpos, double ypos) 
{	
    event -> type 				= MouseScroll;
    event -> categoryFlags 		= EventCategoryMouse | EventCategoryInput;
    event -> getName 			= MouseScrollEvent_getName;
	event -> getCategoryFlags 	= MouseEvent_getCategoryFlags;
    event -> debug 				= MouseScrollEvent_debug;
    event -> handled			= false;
	
	mouseEvent.coordinates.mouseX = xpos;
	mouseEvent.coordinates.mouseY = ypos;
	
	setCurrentEvent(event);
	
	return event;
}

double getXOffset() { return mouseEvent.coordinates.mouseX; }
double getYOffset() { return mouseEvent.coordinates.mouseY; }

// MouseButtonPressedEvent

EVENT_CLASS_TYPE(MouseButtonPressed)

static const char* MouseButtonPressedEvent_debug() 
{
    snprintf(buffer, sizeof(buffer), "@input: mouse button pressed: %d", mouseEvent.button);
    return buffer;
}

Event* createMouseButtonPressedEvent(int button) 
{	
    event -> type 				= MouseButtonPressed;
    event -> categoryFlags 		= EventCategoryMouse | EventCategoryInput;
    event -> getName 			= MouseButtonPressedEvent_getName;
	event -> getCategoryFlags 	= MouseEvent_getCategoryFlags;
    event -> debug 				= MouseButtonPressedEvent_debug;
    event -> handled			= false;
	
	mouseEvent.button = button;
	
	setCurrentEvent(event);
	return event;
	
}

// MouseButtonReleasedEvent

EVENT_CLASS_TYPE(MouseButtonReleased)

static const char* MouseButtonReleasedEvent_debug() 
{
    snprintf(buffer, sizeof(buffer), "@input: mouse button released: %d", mouseEvent.button);
    return buffer;
}

Event* createMouseButtonReleasedEvent(int button) 
{	
    event -> type 				= MouseButtonReleased;
    event -> categoryFlags 		= EventCategoryMouse | EventCategoryInput;
    event -> getName 			= MouseButtonReleasedEvent_getName;
	event -> getCategoryFlags 	= MouseEvent_getCategoryFlags;
    event -> debug 				= MouseButtonReleasedEvent_debug;
    event -> handled			= false;
	
	mouseEvent.button = button;
	
	setCurrentEvent(event);
	return event;
}