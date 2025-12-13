#ifndef ENTRY_H
#define ENTRY_H

#include "core/layers.h"

extern LayerStack*  stack;
extern bool         Running;
extern bool         Minimised;

extern void onEvent(Event* );
extern void onWindowClose(Event* );

#endif // ENTRY_H
