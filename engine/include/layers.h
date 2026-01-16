#ifndef LAYERS_H    
#define LAYERS_H

#include <stdbool.h>

#include "defines.h"
#include "event_include.h"
#include "vector_include.h"

// Layer structure
typedef enum LayerID {
    LAYER_RENDER = 1,
    LAYER_GAMEPLAY,
    LAYER_CAMERA,
    LAYER_UI,
    LAYER_AUDIO,
    LAYER_WINDOW_CLOSE,
    LAYER_DEBUG
} LayerID;

typedef struct {
    char name[32];                      // Name of the layer for debugging
    void* data;
    void (*onDetatch)(void* );          // Function pointer for layer attachment
    void (*update)(float deltaTime);    // Function pointer for updating the layer
    void (*render)();                   // Function pointer for rendering the layer
    void (*onEvent)(Event* e);          // Function pointer for handling events
    LayerID id;                         // Unique identifier for the layer
    bool event_enabled;
} Layer;

typedef Vector LayerStack;

REN_API LayerStack* create_layer_stack();
REN_API void destroy_layer_stack(LayerStack* );

REN_API void push_layer(LayerStack* , Layer );

REN_API void enable_layer_event(Layer* );
REN_API void disable_layer_event(Layer* );

REN_API void update_layers(LayerStack* , float);
REN_API void render_layers(LayerStack* );

#endif // LAYERS_H