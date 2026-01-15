#ifndef ENTRY_H
#define ENTRY_H

#include "../include/layers.h"

REN_API extern LayerStack*  stack;

REN_API bool  Running();
REN_API bool  Minimised();

extern void onEvent(Event* );
REN_API void onWindowClose(Event* );
REN_API void engine_init(const char* , int , int );

#endif // ENTRY_H
