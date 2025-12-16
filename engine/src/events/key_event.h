#ifndef KEY_EVENT_H
#define KEY_EVENT_H

#include "events/event.h"

#define NO_REPEAT	-1

typedef struct KeyEvent {
    Event event;
    int key_code;
	int repeat_count;
} KeyEvent;

extern Event* createKeyPressedEvent(int , int );
extern Event* createKeyReleasedEvent(int ); 

extern int getRepeatCount(); //not working as repeat_count does not increment

#endif // KEY_EVENT_H