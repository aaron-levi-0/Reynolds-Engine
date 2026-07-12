#include "input.h"

#include "core/window_internals.h"
#include <GLFW/glfw3.h>
#include "keycodes.h"

static double xpos, ypos;

bool isKeyPressed(int keycode)
{
	GLFWwindow* window = (GLFWwindow* )GetNativeWindow();
	int state = glfwGetKey(window, keycode);
	
	return state == MOUSE_PRESS || state == MOUSE_REPEAT;
}

bool isMouseButtonPressed(int button)
{
	GLFWwindow* window = (GLFWwindow* )GetNativeWindow();
	int state = glfwGetMouseButton(window, button);
	
	return state == MOUSE_PRESS;
}

void setMousePos()
{
	GLFWwindow* window = (GLFWwindow* )GetNativeWindow();	
	glfwGetCursorPos(window, &xpos, &ypos);
}

void getMousePos(double* x, double* y)
{
	setMousePos();
	
	*x = xpos;
	*y = ypos;
}