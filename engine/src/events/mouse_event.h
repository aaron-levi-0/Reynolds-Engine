#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H

#include "events/event.h"

typedef struct MouseEvent {
    Event event;
	union 
	{
		struct {
			double mouseX;
			double mouseY;
		} coordinates;
		
		int button;
	};
} MouseEvent;

extern Event* createMouseMovedEvent(double , double );
extern Event* createMouseScrollEvent(double , double );
extern Event* createMouseButtonPressedEvent(int );
extern Event* createMouseButtonReleasedEvent(int ); 

extern double getXOffset();
extern double getYOffset();

#endif // MOUSE_EVENT_H