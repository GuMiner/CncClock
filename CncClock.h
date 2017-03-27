#pragma once
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm\mat4x4.hpp>
#include <glm\gtc\quaternion.hpp>
#include "shaders\ShaderFactory.h"
#include "ImguiRenderer.h"
#include "Viewer.h"

class CncClock
{
    GLFWwindow* window;
    ShaderFactory shaderFactory;
    ImguiRenderer guiRenderer;
    Viewer viewer;

    // Top-level display items.
    float fpsTimeAggregated;
    int fpsFramesCounted;
    float lastFrameRate;

    void UpdateFps(float frameTime);

    bool LoadCoreGlslGraphics();
    void LogSystemSetup();

    void HandleEvents(bool& focusPaused, bool& escapePaused);
    void Update(float currentTime, float frameTime);
    void Render(glm::mat4& viewMatrix);

public:
    CncClock();

    bool Initialize();
    void Deinitialize();

    bool LoadGraphics();
    void UnloadGraphics();
    
    bool Run();
};