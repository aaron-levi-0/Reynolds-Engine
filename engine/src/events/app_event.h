#ifndef APP_EVENT_H
#define APP_EVENT_H

#include "events/event.h"

// Key Pressed Event
typedef struct WindowEvent {
    Event event;
	
	union {
		struct size 	{ int width, height; };	
		struct position { int xpos, ypos; };
	};
	
} WindowEvent;

extern Event* createWindowResizeEvent(unsigned int , unsigned int );
extern Event* createWindowMovedEvent(int, int );
extern Event* createWindowCloseEvent();

#endif // APP_EVENT_H