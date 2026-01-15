#include "key_event.h"

#include <stdio.h>

KeyEvent keyEvent;
Event* event = &keyEvent.event;

static char buffer[256];
 
// FUNCTION IMPLEMENTATIONS //

int KeyEvent_getCategoryFlags() { return EventCategoryKeyboard | EventCategoryInput; }

// KeyPressedEvent

EVENT_CLASS_TYPE(KeyPressed)

int getRepeatCount() { return keyEvent.repeat_count; }

const char* KeyPressedEvent_debug() 
{
    snprintf(buffer, sizeof(buffer), "@input: keyCode = %d (pressed %d times)", keyEvent.key_code, keyEvent.repeat_count);
    return buffer;
}

Event* createKeyPressedEvent(int keyCode, int repeatCount) 
{	
    event -> type 				= KeyPressed;
    event -> categoryFlags 		= EventCategoryKeyboard | EventCategoryInput;
    event -> getName 			= KeyPressedEvent_getName;
    event -> getCategoryFlags 	= KeyEvent_getCategoryFlags;
    event -> debug 				= KeyPressedEvent_debug;
    event -> handled			= false;
	
	keyEvent.key_code 		= keyCode;
	keyEvent.repeat_count 	= repeatCount;
	
	setCurrentEvent(event);
	
	return event;
}

// KeyReleasedEvent

EVENT_CLASS_TYPE(KeyReleased)

const char* KeyReleasedEvent_debug() 
{
    snprintf(buffer, sizeof(buffer), "@input: keyCode = %d", keyEvent.key_code);
    return buffer;
}

Event* createKeyReleasedEvent(int keyCode) 
{
    event -> type 				= KeyReleased;
    event -> categoryFlags 		= EventCategoryKeyboard | EventCategoryInput;
    event -> getName 			= KeyReleasedEvent_getName;
    event -> getCategoryFlags 	= KeyEvent_getCategoryFlags;
    event -> debug 				= KeyReleasedEvent_debug;
    event -> handled			= false;
	
	keyEvent.key_code 		= keyCode;
	keyEvent.repeat_count 	= NO_REPEAT;

	setCurrentEvent(event);	
	return event;
}

/* Event CreateKeyEvent(EventType type, int categoryFlags) 
{
    Event event = {type, categoryFlags, DefaultGetName, DefaultIsInCategory};
    return event;
} */
