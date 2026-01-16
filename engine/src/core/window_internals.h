#ifndef WINDOW_INTERNALS_H
#define WINDOW_INTERNALS_H

#include "window.h"

//this is used to generalise the window process so that the application is not entirely
//dependent on glfw. The variables are passed onto glfw if called upon by the library.

struct Window
{
	const char* title;
	unsigned int width, height;
	int xpos, ypos;
	bool VSync;
	
	EventCallbackFn EventCallback;
};

extern void* GetNativeWindow();

#endif // WINDOW_INTERNALS_H