#include <iostream>
#include "logging\Logger.h"
#include "Input.h"

bool Input::hasFocus = true;
GLFWwindow* Input::window = nullptr;
Viewer* Input::viewer = nullptr;

std::set<int> Input::pressedKeys;
std::set<int> Input::pressedMouseButtons;

std::vector<IGlfwMouseHandler*> Input::additionalMouseHandlers;
std::vector<IGlfwKeyHandler*> Input::additionalKeyHandlers;

void Input::SetupErrorCallback()
{
    glfwSetErrorCallback(Input::LogGlfwErrors);
}

void Input::Setup(GLFWwindow* window, Viewer* viewer)
{
    Input::window = window;
    Input::viewer = viewer;

    // Call the resize callback initially to keep our viewport accurate.
    Input::GlfwWindowResizeCallbacks(window, viewer->ScreenWidth, viewer->ScreenHeight);

    glfwSetKeyCallback(window, Input::GlfwKeyCallback);
    glfwSetCharCallback(window, Input::GlfwCharCallback);
    glfwSetWindowCloseCallback(window, Input::GlfwWindowCloseCallbacks);
    glfwSetWindowFocusCallback(window, Input::GlfwWindowFocusCallbacks);
    glfwSetWindowSizeCallback(window, Input::GlfwWindowResizeCallbacks);
    glfwSetMouseButtonCallback(window, Input::GlfwMouseButtonCallbacks);
    glfwSetScrollCallback(window, Input::GlfwScrollCallback);
}

void Input::AddMouseHandler(IGlfwMouseHandler* mouseHandler)
{
    additionalMouseHandlers.push_back(mouseHandler);
}

void Input::AddKeyHandler(IGlfwKeyHandler* keyHandler)
{
    additionalKeyHandlers.push_back(keyHandler);
}

// Logs any errors from GLFW
void Input::LogGlfwErrors(int error, const char* description)
{
    Logger::LogError("Error code: ", error, ". Description: ", description);
}

void Input::GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        pressedKeys.insert(key);

        // Perform direct actions.
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        pressedKeys.erase(key);
    }

    for (IGlfwKeyHandler* handler: additionalKeyHandlers)
    {
        handler->GlfwKeyCallback(window, key, scancode, action, mods);
    }
}

void Input::GlfwCharCallback(GLFWwindow * window, unsigned int character)
{
    for (IGlfwKeyHandler* handler : additionalKeyHandlers)
    {
        handler->GlfwCharCallback(window, character);
    }
}

void Input::GlfwMouseButtonCallbacks(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        pressedMouseButtons.insert(button);
    }
    else if (action == GLFW_RELEASE)
    {
        pressedMouseButtons.erase(button);
    }

    for (IGlfwMouseHandler* handler : additionalMouseHandlers)
    {
        handler->GlfwMouseButtonCallbacks(window, button, action, mods);
    }
}

void Input::GlfwScrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    for (IGlfwMouseHandler* handler : additionalMouseHandlers)
    {
        handler->GlfwScrollCallback(window, xoffset, yoffset);
    }
}

void Input::GlfwWindowCloseCallbacks(GLFWwindow* window)
{
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void Input::GlfwWindowFocusCallbacks(GLFWwindow* window, int focused)
{
    hasFocus = focused == GLFW_TRUE;
}

void Input::GlfwWindowResizeCallbacks(GLFWwindow* window, int width, int height)
{
    // Letterboxing is done at the top and bottom.
    float necessaryWidth = (float)height * viewer->GetAspectRatio();
    if (necessaryWidth > width)
    {
        // Letterbox the top and the bottom of the screen so that the aspect ratio is met
        float effectiveHeight = (float)width / viewer->GetAspectRatio();
        float heightDelta = ((float)height - effectiveHeight) / 2.0f;
        glViewport(0, (int)heightDelta, (GLsizei)width, (GLsizei)effectiveHeight);
    }
    else
    {
        // Letterbox the left and the right so that the aspect ratio is met.
        float widthDelta = ((float)width - necessaryWidth) / 2.0f;
        glViewport((GLint)widthDelta, (GLint)0, (GLsizei)necessaryWidth, (GLsizei)height);
    }

    viewer->SetScreenSize(width, height);
}

// Returns true if a key was pressed, false otherwise.
bool Input::IsKeyPressed(int keyId)
{
    return pressedKeys.find(keyId) != pressedKeys.end();
}

bool Input::IsMouseButtonPressed(int mouseButton)
{
    return pressedMouseButtons.find(mouseButton) != pressedMouseButtons.end();
}

glm::ivec2 Input::GetMousePos()
{
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    return glm::ivec2((int)xPos, (int)yPos);
}