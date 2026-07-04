#include "event.h"

#include <stdlib.h>
#include "logging.h"

const char* getEventName(EventType t) 
{
    switch (t) 
    {
        case WindowResize:          return "WindowResize";
        case WindowMoved:           return "WindowMoved";
        case WindowClose:           return "WindowClose";
        case KeyPressed:            return "KeyPressed";
        case KeyReleased:           return "KeyReleased";
        case MouseButtonPressed:    return "MouseButtonPressed";
        case MouseButtonReleased:   return "MouseButtonReleased";
        case MouseMoved:            return "MouseMoved";
        case MouseScroll:           return "MouseScroll";
        case NoEvent:               return "NoEvent";
        default:                    return "Unknown";
    }
}

// WINDOW EVENTS
Event* createWindowResizeEvent (uint32_t width, uint32_t height)
{
    Event* event = malloc(sizeof(Event));

    if (!event)
    {
        REYNOLDS_ERROR("Failed to allocate memory for winodw resize event.");
        return NULL;
    }

    event -> type 			= WindowResize;
    event -> handled		= false;
	event -> resize.width	= width;
	event -> resize.height 	= height;
	
	return event;
}

Event* createWindowMovedEvent(uint32_t xpos, uint32_t ypos)
{	
    Event* event = malloc(sizeof(Event));

    if (!event)
    {
        REYNOLDS_ERROR("Failed to allocate memory for winodw moved event.");
        return NULL;
    }

    event -> type 	    = WindowMoved;
    event -> handled    = false;
	event -> moved.x	= xpos;
	event -> moved.y 	= ypos;
	
	return event;
}

Event* createWindowCloseEvent()
{	
    Event* event = malloc(sizeof(Event));

    if (!event)
    {
        REYNOLDS_ERROR("Failed to allocate memory for winodw closed event.");
        return NULL;
    }

    event -> type 	  = WindowClose;
    event -> handled  = false;
	
	return event;
}

// KEY EVENTS
#define NO_KEY_REPEAT	-1

Event* createKeyPressedEvent(int keyCode, int repeatCount) 
{
    Event* event = malloc(sizeof(Event));

    if (!event)
    {
        REYNOLDS_ERROR("Failed to allocate memory for key pressed event.");
        return NULL;
    }

    event -> type 		= KeyPressed;
    event -> handled	= false;
	event -> key.code 	= keyCode;
	event -> key.repeat = repeatCount;
	
	return event;
}

Event* createKeyReleasedEvent(int keyCode) 
{
    Event* event = malloc(sizeof(Event));

    if (!event)
    {
        REYNOLDS_ERROR("Failed to allocate memory for key released event.");
        return NULL;
    }

    event -> type 		= KeyReleased;
    event -> handled	= false;
	event -> key.code 	= keyCode;
	event -> key.repeat = NO_KEY_REPEAT;

	return event;
}

uint16_t getRepeatCount() 
{
    static uint16_t repeatCount = 0;
    return repeatCount++;
}

// MOUSE EVENTS
Event* createMouseMovedEvent(double xpos, double ypos) 
{	
    Event* event = malloc(sizeof(Event));

    if (!event)
    {
        REYNOLDS_ERROR("Failed to allocate memory for mouse moved event.");
        return NULL;
    }

    event -> type 		= MouseMoved;
    event -> handled	= false;
	event -> mouse.x    = xpos;
	event -> mouse.y    = ypos;
	
	return event;
}

Event* createMouseScrollEvent(double xpos, double ypos) 
{	
    Event* event = malloc(sizeof(Event));

    if (!event)
    {
        REYNOLDS_ERROR("Failed to allocate memory for mouse scroll event.");
        return NULL;
    }

    event -> type 		= MouseScroll;
    event -> handled	= false;
	event -> mouse.x    = xpos;
	event -> mouse.y    = ypos;
	
	return event;
}

Event* createMouseButtonPressedEvent(int button) 
{	
    Event* event = malloc(sizeof(Event));

    if (!event)
    {
        REYNOLDS_ERROR("Failed to allocate memory for mouse button pressed event.");
        return NULL;
    }

    event -> type 			= MouseButtonPressed;
    event -> handled		= false;
	event -> button.button  = button;

	return event;	
}

Event* createMouseButtonReleasedEvent(int button) 
{	
    Event* event = malloc(sizeof(Event));

    if (!event)
    {
        REYNOLDS_ERROR("Failed to allocate memory for mouse button released event.");
        return NULL;
    }

    event -> type 			= MouseButtonReleased;
    event -> handled		= false;
	event -> button.button  = button;

	return event;
}

double getMouseX(Event* event) 
{
    if (event -> type == MouseMoved || event -> type == MouseScroll) 
        return event -> mouse.x;

    return 0.0; // Return 0 if the event is not a mouse event
}

double getMouseY(Event* event) 
{
    if (event -> type == MouseMoved || event -> type == MouseScroll) 
        return event -> mouse.y;

    return 0.0; // Return 0 if the event is not a mouse event
}