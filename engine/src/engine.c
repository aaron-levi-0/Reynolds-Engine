#include "engine.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/version.h>

void EngineDependencies()
{
	REYNOLDS_INFO("Status: Using GLEW %s", glewGetString(GLEW_VERSION));
	REYNOLDS_INFO("Status: Using GLFW %s", glfwGetVersionString());
	REYNOLDS_INFO("Status: Using OpenGL %s", glGetString(GL_VERSION));
	REYNOLDS_INFO("Status: Using CGLM %d.%d.%d", CGLM_VERSION_MAJOR, CGLM_VERSION_MINOR, CGLM_VERSION_PATCH);
}