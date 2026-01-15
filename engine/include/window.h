#ifndef WINDOW_H
#define WINDOW_H

#include "events/event.h"

//this is used to generalise the window process so that the application is not entirely
//dependent on glfw. The variables are passed onto glfw if called upon by the library.

typedef void (*EventCallbackFn)(Event* );
typedef struct Window Window;

REN_API void setVSync(bool );

REN_API unsigned int getWindowWidth();
REN_API unsigned int getWindowHeight();

REN_API Window* create_window(const char* , unsigned int , unsigned int ); 
REN_API void update_window();
REN_API void close_window();

REN_API void SetEventCallback(EventCallbackFn );
REN_API void load_icon(const char* path);

//#define CREATE_WIN 	create_window("Reynolds Engine", 970, 970)

#endif // WINDOW_H