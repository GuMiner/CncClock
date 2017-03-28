#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <Box2D\Common\b2Draw.h>
#include <glm\gtx\transform.hpp>
#include <imgui\imgui.h>
#include "logging\Logger.h"
#include "Input.h"
#include "version.h"
#include "CncClock.h"

#pragma comment(lib, "opengl32")
#pragma comment(lib, "lib/Box2D.lib")
#pragma comment(lib, "lib/glfw3.lib")
#pragma comment(lib, "lib/glew32.lib")
#pragma comment(lib, "lib/imgui.lib")
#pragma comment(lib, "lib/sfml-system")

CncClock::CncClock()
    : shaderFactory(), viewer(), guiRenderer(),
      fpsTimeAggregated(0.0f), fpsFramesCounted(0), lastFrameRate(60.0f),
      world(b2Vec2(0.0f, -10.0f)),
      clock(), designMenu(&clock)
{
}

void CncClock::LogSystemSetup()
{
    Logger::Log("OpenGL vendor: ", glGetString(GL_VENDOR), ", version ", glGetString(GL_VERSION), ", renderer ", glGetString(GL_RENDERER));
    Logger::LogDebug("OpenGL extensions: ", glGetString(GL_EXTENSIONS));

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

    window = glfwCreateWindow(viewer.ScreenWidth, viewer.ScreenHeight, "CNC Clock", nullptr, nullptr);
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
    glBlendEquation(GL_FUNC_ADD);
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

void CncClock::HandleEvents(bool& focusPaused)
{
    glfwPollEvents();
    focusPaused = !Input::hasFocus;
}

void CncClock::UpdateFps(float frameTime)
{
    ++fpsFramesCounted;
    fpsTimeAggregated += frameTime;
    if (fpsTimeAggregated > 1.0f)
    {
        lastFrameRate = (float)fpsFramesCounted / (float)fpsTimeAggregated;
        fpsTimeAggregated = 0;
        fpsFramesCounted = 0;
    }

    ImGui::Begin("Stats", nullptr, ImVec2(100, 100), 0.50f);
    ImGui::SetCursorPos(ImVec2(5, 20));
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.1f", lastFrameRate);
    ImGui::End();
}

void CncClock::Update(float currentTime, float frameTime)
{
    guiRenderer.Update(currentTime, frameTime); // Must be before any IMGUI commands are passed in.
    viewer.Update(frameTime);
    
    glm::ivec2 iMousePos = Input::GetMousePos();
    glm::vec2 gridPos = viewer.GetGridPos(iMousePos);
    ImGui::SetNextWindowPos(ImVec2((float)iMousePos.x + 20.0f, (float)iMousePos.y));
    ImGui::Begin("Mouse Pos", nullptr, ImVec2(100, 100), 0.0f, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::SetCursorPos(ImVec2(0, 0));
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.1f, %.1f", gridPos.x, gridPos.y);
    ImGui::End();

    UpdateFps(frameTime);

    designMenu.Update(&world);
    clock.Update(&viewer);

    world.Step(1.0f / 60.0f, 16, 6);
    
    world.DrawDebugData();
    debugRenderer->Update();
}

void CncClock::Render(glm::mat4& viewMatrix)
{
    glm::mat4 projectionMatrix = viewer.perspectiveMatrix * viewMatrix;

    // Clear the screen (and depth buffer) before any rendering begins.
    const GLfloat color[] = { 0, 0.2f, 0, 1 };
    const GLfloat one = 1.0f;
    glClearBufferfv(GL_COLOR, 0, color);
    glClearBufferfv(GL_DEPTH, 0, &one);

    debugRenderer->Render(projectionMatrix);
    guiRenderer.Render();
}

bool CncClock::LoadGraphics()
{
    if (!LoadCoreGlslGraphics())
    {
        return false;
    }

    if (!LineRenderer::LoadProgram(&shaderFactory))
    {
        return false;
    }

    if (ImGui::GetIO().Fonts->AddFontFromFileTTF("./fonts/DejaVuSans.ttf", 15.f) == nullptr)
    {
        Logger::LogError("Unable to load the custom font for IMGUI to use.");
        return false;
    }

    if (!guiRenderer.LoadImGui(window, &shaderFactory))
    {
        return false;
    }

    debugRenderer = new b2DebugDrawRenderer();

    // Add some test data
    // Pyramids demo from Box2D tests
    {
        b2BodyDef bd;
        b2Body* ground = world.CreateBody(&bd);

        b2EdgeShape shape;
        shape.Set(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));
        ground->CreateFixture(&shape, 0.0f);
    }

    {
        float32 a = 0.5f;
        b2PolygonShape shape;
        shape.SetAsBox(a, a*2);

        b2Vec2 x(-7.0f, 0.75f);
        b2Vec2 y;
        b2Vec2 deltaX(0.5625f, 1.25f);
        b2Vec2 deltaY(1.125f, 0.0f);

        int e_count = 6;
        for (int32 i = 0; i < e_count; ++i)
        {
            y = x;

            for (int32 j = i; j < e_count; ++j)
            {
                b2BodyDef bd;
                bd.type = b2_dynamicBody;
                bd.position = y;
                b2Body* body = world.CreateBody(&bd);
                body->CreateFixture(&shape, 5.0f);

                y += deltaY;
            }

            x += deltaX;
        }
    }

    debugRenderer->SetFlags(b2Draw::e_shapeBit | b2Draw::e_jointBit);
    world.SetDebugDraw(debugRenderer);

    return true;
}

void CncClock::UnloadGraphics()
{
    delete debugRenderer;
    guiRenderer.UnloadImGui();

    glfwDestroyWindow(window);
    window = nullptr;
}

bool CncClock::Run()
{
    float gameTime = 0;
    float lastFrameTime = 0.06f;

    bool focusPaused = false;

    while (!glfwWindowShouldClose(window))
    {
        HandleEvents(focusPaused);

        // Run the game and render if not paused.
        if (!focusPaused)
        {
            Update(gameTime, lastFrameTime);

            Render(viewer.viewMatrix);
            glfwSwapBuffers(window);
        }

        float now = (float)glfwGetTime();
        lastFrameTime = (now - gameTime);
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
    Logger::Setup("cncClock.log", true);
    Logger::Log("CncClock ", AutoVersion::MAJOR_VERSION, ".", AutoVersion::MINOR_VERSION);

    CncClock* cncClock = new CncClock();
    if (!cncClock->Initialize())
    {
        Logger::LogError("CncClock initialization failed!");
    }
    else
    {
        Logger::Log("Basic Initialization complete!");
        if (!cncClock->LoadGraphics())
        {
            Logger::LogError("CncClock graphics initialization failed!");
        }
        else
        {
            Logger::Log("Graphics Initialized!");
            if (!cncClock->Run())
            {
                Logger::LogError("CncClock operation failed!");
            }

            cncClock->UnloadGraphics();
        }

        cncClock->Deinitialize();
    }

    delete cncClock;
    Logger::Log("Done.");
    Logger::Shutdown();
    return 0;
}