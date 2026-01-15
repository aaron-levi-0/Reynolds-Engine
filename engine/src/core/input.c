#include "../include/input.h"

#include "core/window_internals.h"
#include <GLFW/glfw3.h>

static double xpos, ypos;

bool isKeyPressed(int keycode)
{
	GLFWwindow* window = (GLFWwindow* )GetNativeWindow();
	int state = glfwGetKey(window, keycode);
	
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool isMouseButtonPressed(int button)
{
	GLFWwindow* window = (GLFWwindow* )GetNativeWindow();
	int state = glfwGetMouseButton(window, button);
	
	return state == GLFW_PRESS;
}

void setMousePos()
{
	GLFWwindow* window = (GLFWwindow* )GetNativeWindow();	
	glfwGetCursorPos(window, &xpos, &ypos);
}

double getMouseX() { return xpos; }
double getMouseY() { return ypos; }