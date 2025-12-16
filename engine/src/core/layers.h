#ifndef LAYERS_H    
#define LAYERS_H

#include "glx.h"
#include "events/event.h"
#include "utils/vector.h"

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

typedef struct {
    Vector layers;
} LayerStack;

extern LayerStack* create_layer_stack();
extern void destroy_layer_stack(LayerStack* );

extern void push_layer(LayerStack* , Layer );

REN_API void enable_layer_event(Layer* );
REN_API void disable_layer_event(Layer* );

extern void update_layers(LayerStack* , float);
extern void render_layers(LayerStack* );
extern void handle_layer_events(LayerStack* , Event* );

#endif // LAYERS_H