#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Defines methods deriving classes must implement to handle GLFW mouse callbacks.
class IGlfwMouseHandler
{
public:
    virtual void GlfwMouseButtonCallbacks(GLFWwindow* window, int button, int action, int mods) = 0;
    virtual void GlfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset) = 0;
};