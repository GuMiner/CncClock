#pragma once
#include <set>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Viewer.h"
#include "IGlfwMouseHandler.h"
#include "IGlfwKeyHandler.h"

class Input
{
    // The window.
    static GLFWwindow* window;

    // The viewer controlling graphical settings
    static Viewer* viewer;

    // List of pressed keys.
    static std::set<int> pressedKeys;

    // Same as the key sets, but for mouse buttons.
    static std::set<int> pressedMouseButtons;

    static std::vector<IGlfwMouseHandler*> additionalMouseHandlers;
    static std::vector<IGlfwKeyHandler*> additionalKeyHandlers;

    // Logs any errors from GLFW
    static void LogGlfwErrors(int error, const char* description);

    // Handles GLFW key callbacks.
    static void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void GlfwCharCallback(GLFWwindow* window, unsigned int character);

    // Handles GLFW mouse callbacs
    static void GlfwMouseButtonCallbacks(GLFWwindow* window, int button, int action, int mods);
    static void GlfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    // Handles GLFW window callbacks.
    static void GlfwWindowCloseCallbacks(GLFWwindow* window);
    static void GlfwWindowFocusCallbacks(GLFWwindow* window, int focused);
    static void GlfwWindowResizeCallbacks(GLFWwindow* window, int width, int height);

public:
    static bool hasFocus;

    static void SetupErrorCallback();

    // Sets callbacks this class uses
    static void Setup(GLFWwindow* window, Viewer* viewer);
    static void AddMouseHandler(IGlfwMouseHandler* mouseHandler);
    static void AddKeyHandler(IGlfwKeyHandler* keyHandler);

    // Returns true if a key was pressed, false otherwise.
    static bool IsKeyPressed(int keyId);

    // Same as IsKeyPressed ans IsKeyTyped, but with mouse buttons, respectively.
    static bool IsMouseButtonPressed(int mouseButton);

    static glm::ivec2 GetMousePos();
};

