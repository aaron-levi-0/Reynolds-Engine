#ifndef EVENT_H
#define EVENT_H

#include "event_include.h"
 
// Events  are currently blocking, meaning when an event occurs it
// immediately gets dispatched and must be dealt with right then an there.
// For the future, a better strategy might be to buffer events in an event
// bus and process them during the "event" part of the update stage.

extern const char* getEventName(EventType t);

extern Event* createWindowResizeEvent(uint32_t width, uint32_t height);
extern Event* createWindowMovedEvent(uint32_t xpos, uint32_t ypos);
extern Event* createWindowCloseEvent();

extern Event* createKeyPressedEvent(int keyCode, int repeatCount);
extern Event* createKeyReleasedEvent(int keyCode); 
extern uint16_t getRepeatCount();

extern Event* createMouseMovedEvent(double xpos, double ypos);
extern Event* createMouseScrollEvent(double xpos, double ypos);
extern Event* createMouseButtonPressedEvent(int button, double xpos, double ypos);
extern Event* createMouseButtonReleasedEvent(int button, double xpos, double ypos); 

#endif // EVENT_H