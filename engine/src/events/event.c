#include "event.h"

#include <stdlib.h>

// Pointer to the most recent event
static Event* currentEvent = NULL;

const char* DefaultGetName() {
    return "Unknown Event";
}

bool isInCategory(Event* event, EventCategory category) 
{
    return (event->categoryFlags & category) != 0;
}

// Set the current event
void setCurrentEvent(Event* event) { currentEvent = event; }

