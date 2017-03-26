#include <iostream>
#include "logging\Logger.h"
#include "Input.h"

bool Input::hasFocus = true;
GLFWwindow* Input::window = nullptr;
Viewer* Input::viewer = nullptr;

std::set<int> Input::pressedKeys;
std::set<int> Input::pressedKeysTypeChecked;

std::set<int> Input::pressedMouseButtons;
std::set<int> Input::pressedMouseButtonsTypeChecked;

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
    glfwSetWindowCloseCallback(window, Input::GlfwWindowCloseCallbacks);
    glfwSetWindowFocusCallback(window, Input::GlfwWindowFocusCallbacks);
    glfwSetWindowSizeCallback(window, Input::GlfwWindowResizeCallbacks);
    glfwSetMouseButtonCallback(window, Input::GlfwMouseButtonCallbacks);
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
        pressedKeysTypeChecked.erase(key);
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
        pressedMouseButtonsTypeChecked.erase(button);
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

// Returns true if a key was pressed and then false until the key is released.
bool Input::IsKeyTyped(int keyId)
{
    // We haven't checked if it has been typed already.
    if (pressedKeysTypeChecked.find(keyId) == pressedKeysTypeChecked.end())
    {
        if (IsKeyPressed(keyId))
        {
            pressedKeysTypeChecked.insert(keyId);
            return true;
        }
    }

    return false;
}

bool Input::IsMouseButtonPressed(int mouseButton)
{
    return pressedMouseButtons.find(mouseButton) != pressedMouseButtons.end();
}

bool Input::IsMouseButtonClicked(int mouseButton)
{
    // We haven't checked if it has been pressed already.
    if (pressedMouseButtonsTypeChecked.find(mouseButton) == pressedMouseButtonsTypeChecked.end())
    {
        if (IsMouseButtonPressed(mouseButton))
        {
            pressedMouseButtonsTypeChecked.insert(mouseButton);
            return true;
        }
    }

    return false;
}

glm::ivec2 Input::GetMousePos()
{
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    return glm::ivec2((int)xPos, (int)yPos);
}