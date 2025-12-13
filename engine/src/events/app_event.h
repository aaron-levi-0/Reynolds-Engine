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