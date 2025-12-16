#ifndef ENTRY_H
#define ENTRY_H

#include "core/layers.h"

extern LayerStack*  stack;
extern bool         Running;
extern bool         Minimised;

extern void onEvent(Event* );
extern void onWindowClose(Event* );
REN_API void engine_init(const char* , int , int );

#endif // ENTRY_H
