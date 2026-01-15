#include "core/window_internals.h"

#include "events/app_event.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "core/layers_internals.h"
#include "stb_image.h"

#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "../include/logging.h"

Window* window 			= NULL;
GLFWwindow* g_window	= NULL;

static bool GLFW_Initialised = false;

void setVSync(bool enabled)
{
	enabled ? glfwSwapInterval(1) : glfwSwapInterval(0);
	window -> VSync = enabled;
}

void* GetNativeWindow() { return g_window; }

//NOTE: this effectively links the application to the event system, since window is a static variable within this file.
void SetEventCallback(EventCallbackFn callback)
{
	window -> EventCallback = callback;
}

unsigned int getWindowWidth() { return window -> width; }
unsigned int getWindowHeight() { return window -> height; }

static void ErrorCallback(int error, const char* desc)
{
	REYNOLDS_ERROR("GLFW Error (%d): %s", error, desc);
}

static void WindowSizeCallback(GLFWwindow* _window, int width, int height)
{
	Window* win_callbk = (Window* )glfwGetWindowUserPointer(_window); 
	
	win_callbk -> width = width;
	win_callbk -> height = height;
	
	Event* event = createWindowResizeEvent(width, height);
	win_callbk -> EventCallback(event);
}

static void WindowMoveCallback(GLFWwindow* _window, int xpos, int ypos)
{
	Window* win_callbk = (Window* )glfwGetWindowUserPointer(_window); 
	
	win_callbk -> xpos = xpos;
	win_callbk -> ypos = ypos;
	
	Event* event = createWindowMovedEvent(xpos, ypos);
	win_callbk -> EventCallback(event); 
}

static void WindowCloseCallback(GLFWwindow* _window)
{
	Window* win_callbk = (Window* )glfwGetWindowUserPointer(_window); 
	
	Event* event = createWindowCloseEvent();
	win_callbk -> EventCallback(event);
}

static void KeyCallback(GLFWwindow* _window, int key, int scancode, int action, int mods)
{
	Window* win_callbk = (Window* )glfwGetWindowUserPointer(_window); 
	Event* event = NULL;
	scancode 	+= NOT_USED;
	mods 		+= NOT_USED;
	
	switch(action)
	{
		case GLFW_PRESS:
			event = createKeyPressedEvent(key, 0);
			win_callbk -> EventCallback(event);
			break;
		case GLFW_RELEASE:
			event = createKeyReleasedEvent(key);
			win_callbk -> EventCallback(event);
			break;
		case GLFW_REPEAT:
			event = createKeyPressedEvent(key, getRepeatCount());
			win_callbk -> EventCallback(event);
			break;		
	}	
}

static void MouseButtonCallback(GLFWwindow* _window, int button, int action, int mods)
{
	Window* win_callbk = (Window* )glfwGetWindowUserPointer(_window); 
	Event* event = NULL;
	mods += NOT_USED;
	
	switch(action)
	{
		case GLFW_PRESS:
			event = createMouseButtonPressedEvent(button);
			win_callbk -> EventCallback(event);
			break;
		case GLFW_RELEASE:
			event = createMouseButtonReleasedEvent(button);
			win_callbk -> EventCallback(event);
			break;	
	}
}

static void MouseScrollCallback(GLFWwindow* _window, double xOffset, double yOffset)
{
	Window* win_callbk = (Window* )glfwGetWindowUserPointer(_window); 
	
	Event* event = createMouseScrollEvent(xOffset, yOffset);
	win_callbk -> EventCallback(event);
}

static void MousePosCallback(GLFWwindow* _window, double xpos, double ypos)
{
	Window* win_callbk = (Window* )glfwGetWindowUserPointer(_window); 
	
	Event* event = createMouseMovedEvent(xpos, ypos);
	win_callbk -> EventCallback(event);
}

static void init_window()
{
	// Initialize the GLFW library 
	if (!GLFW_Initialised)	//NOTE: we may want to init multiple windows but only need to init GLFW once
	{
		int success = glfwInit();
		ASSERT_FATAL(success, "Failed to initialize GLFW!");
		glfwSetErrorCallback(ErrorCallback);
		GLFW_Initialised = true;
	}
	
	REYNOLDS_INFO("Creating window %s (%u, %u)", window -> title, window -> width, window -> height);
	
    // Create a windowed mode window and its OpenGL context
    g_window = glfwCreateWindow((int)window -> width, (int)window -> height, window -> title, NULL, NULL);
	glfwMakeContextCurrent(g_window);
	glfwSetWindowUserPointer(g_window, window);
	setVSync(true);
	
	// Set GLFW callbacks
	glfwSetWindowSizeCallback(g_window, WindowSizeCallback);
	glfwSetWindowPosCallback(g_window, WindowMoveCallback);
	glfwSetWindowCloseCallback(g_window, WindowCloseCallback);
	
	glfwSetKeyCallback(g_window, KeyCallback);
	glfwSetMouseButtonCallback(g_window, MouseButtonCallback);
	glfwSetScrollCallback(g_window, MouseScrollCallback);
	glfwSetCursorPosCallback(g_window, MousePosCallback);
}

Window* create_window(const char* title, unsigned int width, unsigned int height)
{
	window = malloc(sizeof(Window));
	ASSERT_FATAL(window, "Failed to allocated memory for window!");
	
	window -> title 	= title;
	window -> width 	= width;
	window -> height 	= height;
	
	init_window();

	return window;
}

void update_window()
{
	// Swap front and back buffers
    glfwSwapBuffers(g_window);
		
	// Poll for and process events
	glfwPollEvents();
}

void close_window()
{
	if(window) 
		free(window);
	
	glfwDestroyWindow(g_window);
	glfwTerminate();
}

void load_icon(const char* path)
{
	int ico_width, ico_height, ico_channels;
	unsigned char* icon_data = stbi_load(path, &ico_width, &ico_height, &ico_channels, STBI_rgb_alpha);
	
    if (g_window)
	{
		GLFWimage icon;
		icon.width = ico_width; 
		icon.height = ico_height;
		icon.pixels = (unsigned char*) icon_data;
		glfwSetWindowIcon(g_window, 1, &icon);	
	}
	else	
    {
		REYNOLDS_ERROR("GLFW window failed to initialise!");
		close_window();
		glfwTerminate();
        exit(EXIT_FAILURE);
    }
	
	//UNCLEAR: safe to free now?
	stbi_image_free(icon_data);
}