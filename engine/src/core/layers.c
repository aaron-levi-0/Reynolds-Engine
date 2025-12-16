#include "layers.h"

#define INITIAL_LAYER_CAPACITY 10

// Function to create a new layer stack
LayerStack* create_layer_stack() 
{
    LayerStack* stack = malloc(sizeof(LayerStack));
    if(!stack) 
    {
        REYNOLDS_ERROR("@layers: failed to allocate memory for layer stack!");
        return NULL;
    }
    vector_init(&stack -> layers, sizeof(Layer), INITIAL_LAYER_CAPACITY);
    return stack;
}

// Function to push a layer onto the stack
void push_layer(LayerStack* stack, Layer layer) 
{
    layer.event_enabled = true;
    vector_push_back(&stack -> layers, &layer);
    REYNOLDS_VERBOSE("@layers: pushed layer: %s", layer.name);
}

// Function to pop a layer from the stack
static void pop_layer(LayerStack* stack) 
{
    if (vector_size(&stack -> layers) <= 0) 
    {
        REYNOLDS_VERBOSE("@layers: layer stack is empty! Cannot pop more layers.");
        return;
    }

    Layer* top_layer = (Layer*)vector_at(&stack -> layers, stack -> layers.size - 1);
    REYNOLDS_VERBOSE("@layers: popped layer: %s", top_layer -> name);
    
    if(top_layer -> onDetatch)
        top_layer -> onDetatch(top_layer -> data);

    vector_pop_back(&stack -> layers); // Pop the layer from the vector
}

// Function to destroy a layer stack
void destroy_layer_stack(LayerStack* stack) 
{
    if(stack) 
    {
        while(vector_size(&stack -> layers) > 0) 
            pop_layer(stack);

        free_vector(&stack -> layers);
        free(stack);
        return;
    }
    REYNOLDS_ERROR("@layers: layer stack is NULL!");
}


uint16_t layer_index(LayerStack* stack, Layer layer)
{
    Layer* stack_layer = (Layer*)vector_at(&stack -> layers, 0);

    for(uint16_t index = 0; index < vector_size(&stack -> layers); index++)
    {
        if(stack_layer -> id == layer.id)
            return index;
        
        stack_layer++;
    }

    REYNOLDS_WARN("@layers: layer index could not be resolved!");
    return 0;
}

// Function to enable the onEvent function for a layer
void enable_layer_event(Layer* layer) 
{
    layer -> event_enabled = true;
    REYNOLDS_DEBUG("@layers: enabled onEvent for layer: %s", layer -> name);    
}

// Function to disable the onEvent function for a layer
void disable_layer_event(Layer* layer) 
{
    layer -> event_enabled = false;
    REYNOLDS_DEBUG("@layers: disabled onEvent for layer: %s", layer -> name);    
}

// Function to update all layers in the stack (from bottom to top)
void update_layers(LayerStack* stack, float deltaTime) 
{
    Layer* layer = (Layer*)vector_at(&stack -> layers, 0);
    for (uint16_t i = 0; i < vector_size(&stack -> layers); i++) 
    {
        if (layer -> update)
            layer -> update(deltaTime);
        layer++;
    }
}

// Function to render all layers in the stack (from top to bottom)
void render_layers(LayerStack* stack) 
{
    uint16_t i = vector_size(&stack -> layers);
    Layer* layer = (Layer*)vector_at(&stack -> layers, i - 1);
    while(i > 0)
    {
        if (layer -> render)
            layer -> render();
        layer--;
        i--;
    }
}

// Function to handle events for all layers in the stack (from top to bottom)
void handle_layer_events(LayerStack* stack, Event* e)
{
    uint16_t i = vector_size(&stack -> layers);
    Layer* layer = (Layer*)vector_at(&stack -> layers, i - 1);
    while(i > 0)
    {
        if (layer -> onEvent && layer -> event_enabled)
            layer -> onEvent(e);
        layer--;
        i--;
    }
}