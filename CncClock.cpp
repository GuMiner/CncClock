#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <glm\gtx\transform.hpp>
#include "logging\Logger.h"
#include "Input.h"
#include "version.h"
#include "CncClock.h"

#pragma comment(lib, "opengl32")
#pragma comment(lib, "lib/glfw3.lib")
#pragma comment(lib, "lib/glew32.lib")
#pragma comment(lib, "lib/sfml-system")

CncClock::CncClock() 
    : shaderFactory(), viewer(),
      fpsTimeAggregated(0.0f), fpsFramesCounted(0)
{
}

void CncClock::LogSystemSetup()
{
    Logger::Log("OpenGL vendor: ", glGetString(GL_VENDOR), ", version ", glGetString(GL_VERSION), ", renderer ", glGetString(GL_RENDERER));
    Logger::Log("OpenGL extensions: ", glGetString(GL_EXTENSIONS));

    GLint maxTextureUnits, maxUniformBlockSize;
    GLint maxVertexUniformBlocks, maxFragmentUniformBlocks;
    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &maxVertexUniformBlocks);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &maxFragmentUniformBlocks);
    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &maxUniformBlockSize);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

    Logger::Log("Max Texture Units: ", maxTextureUnits, ", Max Uniform Size: ", (maxUniformBlockSize / 1024), " kB");
    Logger::Log("Max Vertex Uniform Blocks: ", maxVertexUniformBlocks, ", Max Fragment Uniform Blocks: ", maxFragmentUniformBlocks);
    Logger::Log("Max Texture Size: ", maxTextureSize);
}

bool CncClock::LoadCoreGlslGraphics()
{
    // 24 depth bits, 8 stencil bits, 8x AA, major version 4.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_DEPTH_BITS, 16);
    glfwWindowHint(GLFW_STENCIL_BITS, 16);
    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(viewer.ScreenWidth, viewer.ScreenHeight, "Lux", nullptr, nullptr);
    if (!window)
    {
        Logger::LogError("Could not create the GLFW window!");
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    Input::Setup(window, &viewer);

    // Setup GLEW
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        Logger::LogError("GLEW startup failure: ", err, ".");
        return false;
    }

    // Log graphics information for future reference
    LogSystemSetup();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable line, but not polygon smoothing.
    glEnable(GL_LINE_SMOOTH);

    // Let OpenGL shaders determine point sizes.
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Disable face culling so that see-through flat objects and stuff at 1.0 (cube map, text) work.
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CW);

    // Cutout faces that are hidden by other faces.
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    return true;
}

bool CncClock::Initialize()
{
    // Setup GLFW
    if (!glfwInit())
    {
        Logger::LogError("GLFW startup failure");
        return false;
    }

    return true;
}

void CncClock::Deinitialize()
{
    glfwTerminate();
}

void CncClock::HandleEvents(bool& focusPaused, bool& escapePaused)
{
    glfwPollEvents();
    focusPaused = !Input::hasFocus;
    escapePaused = Input::IsKeyTyped(GLFW_KEY_ESCAPE);
}

void CncClock::UpdateFps(float frameTime)
{
    ++fpsFramesCounted;
    fpsTimeAggregated += frameTime;
    if (fpsTimeAggregated > 1.0f)
    {
        std::stringstream framerate;
        framerate << "FPS: " << (float)((float)fpsFramesCounted / fpsTimeAggregated);
        //sentenceManager.UpdateSentence(fpsSentenceId, framerate.str());

        fpsTimeAggregated = 0;
        fpsFramesCounted = 0;
    }
}

// TODO hacky code to remove with a redesign. Still prototyping here...
int gainId = 0;
void CncClock::Update(float currentTime, float frameTime)
{
    viewer.Update(frameTime);
    
    glm::vec2 screenPos = viewer.GetGridPos(Input::GetMousePos());
    std::stringstream mousePos;
    mousePos << screenPos.x << ", " << screenPos.y;
    //sentenceManager.UpdateSentence(mouseToolTipSentenceId, mousePos.str());
    
    UpdateFps(frameTime);
}

void CncClock::Render(glm::mat4& viewMatrix)
{
    glm::mat4 projectionMatrix = viewer.perspectiveMatrix * viewMatrix;

    // Clear the screen (and depth buffer) before any rendering begins.
    const GLfloat color[] = { 0, 0, 0, 1 };
    const GLfloat one = 1.0f;
    glClearBufferfv(GL_COLOR, 0, color);
    glClearBufferfv(GL_DEPTH, 0, &one);

    // Render the FPS and our current data transfer rate in the upper-left corner.
   // float fpsHeight = sentenceManager.GetSentenceDimensions(fpsSentenceId).y;
   // sentenceManager.RenderSentence(fpsSentenceId, viewer.perspectiveMatrix, glm::translate(glm::vec3(-viewer.GetXSize() / 2.0f, viewer.GetYSize() / 2.0f - (dataSpeedHeight + fpsHeight), 0.0f)) * viewMatrix);

    // Render our mouse tool tip ... at the mouse
    glm::vec2 screenPos = viewer.GetGridPos(Input::GetMousePos());
    //sentenceManager.RenderSentence(mouseToolTipSentenceId, viewer.perspectiveMatrix, glm::translate(glm::vec3(screenPos.x, screenPos.y, 0.0f)) * viewMatrix);
}

bool CncClock::LoadGraphics()
{
    if (!LoadCoreGlslGraphics())
    {
        return false;
    }

    // Load FPS and and a mouse tool tip.
    //fpsSentenceId = sentenceManager.CreateNewSentence();
    //sentenceManager.UpdateSentence(fpsSentenceId, "FPS:", 14, glm::vec3(1.0f, 1.0f, 1.0f));

    //mouseToolTipSentenceId = sentenceManager.CreateNewSentence();
   // sentenceManager.UpdateSentence(mouseToolTipSentenceId, "(?,?)", 12, glm::vec3(1.0f, 1.0f, 1.0f));

    return true;
}

void CncClock::UnloadGraphics()
{
    glfwDestroyWindow(window);
    window = nullptr;
}

bool CncClock::Run()
{
    float gameTime = 0;
    float lastFrameTime = 0.06f;
    auto lastQueriedGameTime = std::chrono::high_resolution_clock::now();

    bool focusPaused = false;
    bool escapePaused = false;

    while (!glfwWindowShouldClose(window))
    {
        HandleEvents(focusPaused, escapePaused);

        // Run the game and render if not paused.
        if (!focusPaused && !escapePaused)
        {
            Update(gameTime, lastFrameTime);

            Render(viewer.viewMatrix);
            glfwSwapBuffers(window);
        }

        auto now = std::chrono::high_resolution_clock::now();
        lastFrameTime = std::chrono::duration_cast<std::chrono::duration<float>>((now - lastQueriedGameTime)).count();
        gameTime += lastFrameTime;

        // Delay to run approximately at our maximum framerate.
        long sleepDelay = ((long)1e6 / viewer.MaxFramerate) - (long)(lastFrameTime * 1e6);
        if (sleepDelay > 0)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(sleepDelay));
        }
    }

    return true;
}

int main()
{
    Logger::Setup("lux-log.log", true);
    Logger::Log("Lux ", AutoVersion::MAJOR_VERSION, ".", AutoVersion::MINOR_VERSION);

    CncClock* lux = new CncClock();
    if (!lux->Initialize())
    {
        Logger::LogError("CncClock initialization failed!");
    }
    else
    {
        Logger::Log("Basic Initialization complete!");
        if (!lux->LoadGraphics())
        {
            Logger::LogError("CncClock graphics initialization failed!");
        }
        else
        {
            Logger::Log("Graphics Initialized!");
            if (!lux->Run())
            {
                Logger::LogError("CncClock operation failed!");
            }

            lux->UnloadGraphics();
        }

        lux->Deinitialize();
    }

    delete lux;
    Logger::Log("Done.");
    Logger::Shutdown();
    return 0;
}