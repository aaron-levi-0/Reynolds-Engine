#ifndef ENTRY_H
#define ENTRY_H

#include "renderer.h"
#include "layers.h"

REN_API bool  Running();
REN_API bool  Minimised();

REN_API void onWindowClose(Event* );
REN_API void InitEngine(const char* title, int width, int height);
REN_API void EngineRun(struct Renderer* renderer, LayerStack* stack);
REN_API void EngineShutdown();
REN_API LayerStack* InitLayerStack();

#endif // ENTRY_H
