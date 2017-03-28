#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif
#include <imgui\imgui.h>
#include "logging\Logger.h"
#include "Input.h"
#include "ImguiRenderer.h"

GLFWwindow* ImguiRenderer::window;

ImguiRenderer::ImguiRenderer()
    : mouseWheelPos(0.0)
{
    for (int i = 0; i < 3; i++)
    {
        isMousePress[i] = false;
    }
}

void ImguiRenderer::SetStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = style.GrabRounding = style.ScrollbarRounding = 2.0f;
    style.FramePadding = ImVec2(4, 2);
    style.DisplayWindowPadding = ImVec2(0, 0);
    style.DisplaySafeAreaPadding = ImVec2(0, 0);
}

bool ImguiRenderer::LoadImGui(GLFWwindow* window, ShaderFactory* shaderFactory)
{
    ImguiRenderer::window = window;

    if (!shaderFactory->CreateShaderProgram("imgui", &program.programId))
    {
        Logger::LogError("Unable to load the IMGUI shader!");
        return false;
    }

    program.textureAttributeLocation = glGetUniformLocation(program.programId, "Texture");
    program.projMtxAttributeLocation = glGetUniformLocation(program.programId, "ProjMtx");

    glGenBuffers(1, &program.vbo);
    glGenBuffers(1, &program.elements);

    glGenVertexArrays(1, &program.vao);
    glBindVertexArray(program.vao);
    glBindBuffer(GL_ARRAY_BUFFER, program.vbo);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
    
    io.RenderDrawListsFn = nullptr; 
    io.SetClipboardTextFn = ImguiRenderer::SetClipboardText;
    io.GetClipboardTextFn = ImguiRenderer::GetClipboardText;
    io.ImeWindowHandle = glfwGetWin32Window(ImguiRenderer::window);

    Input::AddKeyHandler(this);
    Input::AddMouseHandler(this);

    // Build texture atlas
    unsigned char* pixels;
    int width, height;

    // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    
    // Upload texture to graphics system
    glGenTextures(1, &program.fontTexture);
    glBindTexture(GL_TEXTURE_2D, program.fontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    // Store our identifier
    io.Fonts->TexID = (void *)(intptr_t)program.fontTexture;

    return true;
}

void ImguiRenderer::UnloadImGui()
{
    glDeleteVertexArrays(1, &program.vao);
    glDeleteBuffers(1, &program.vbo);
    glDeleteBuffers(1, &program.elements);
    
    glDeleteTextures(1, &program.fontTexture);
    ImGui::GetIO().Fonts->TexID = 0;

    ImGui::Shutdown();
}

void ImguiRenderer::Update(float currentTime, float frameTime)
{
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    glfwGetWindowSize(ImguiRenderer::window, &w, &h);
    glfwGetFramebufferSize(ImguiRenderer::window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Setup time step
    io.DeltaTime = frameTime;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
    if (glfwGetWindowAttrib(ImguiRenderer::window, GLFW_FOCUSED))
    {
        double mouse_x, mouse_y;
        glfwGetCursorPos(ImguiRenderer::window, &mouse_x, &mouse_y);
        io.MousePos = ImVec2((float)mouse_x, (float)mouse_y); // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
    }
    else
    {
        io.MousePos = ImVec2(-1, -1);
    }

    for (int i = 0; i < 3; i++)
    {
        io.MouseDown[i] = isMousePress[i] || Input::IsMouseButtonPressed(i);
        isMousePress[i] = false;
    }

    io.MouseWheel = mouseWheelPos;
    mouseWheelPos = 0.0f;

    // Hide OS mouse cursor if ImGui is drawing it
    glfwSetInputMode(ImguiRenderer::window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

    ImGui::NewFrame();
}

void ImguiRenderer::Render()
{
    ImGui::Render(); // 'Finalize' for render -- the actual rendering is performed from here on down.
    ImDrawData *drawData = ImGui::GetDrawData();
    
    // Avoid rendering when minimized
    ImGuiIO& io = ImGui::GetIO();
    if (io.DisplaySize.x == 0 || io.DisplaySize.y == 0)
    {
        return;
    }

    drawData->ScaleClipRects(io.DisplayFramebufferScale);

    // Backup GL state
    GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glActiveTexture(GL_TEXTURE0);

    // Setup viewport, orthographic projection matrix
    glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
    const float ortho_projection[4][4] =
    {
        { 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        { -1.0f,                  1.0f,                   0.0f, 1.0f },
    };

    glUseProgram(program.programId);
    glUniform1i(program.textureAttributeLocation, 0);
    glUniformMatrix4fv(program.projMtxAttributeLocation, 1, GL_FALSE, &ortho_projection[0][0]);
    glBindVertexArray(program.vao);

    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = drawData->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer(GL_ARRAY_BUFFER, program.vbo);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, program.elements);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

        for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
        {
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(io.DisplaySize.y - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    // Restore modified GL state
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
}

ImguiRenderer::~ImguiRenderer()
{
}

void ImguiRenderer::GlfwKeyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
    {
        io.KeysDown[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        io.KeysDown[key] = false;
    }
    
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void ImguiRenderer::GlfwCharCallback(GLFWwindow * window, unsigned int character)
{
    ImGuiIO& io = ImGui::GetIO();
    if (character > 0 && character < 0x10000)
    {
        io.AddInputCharacter((unsigned short)character);
    }
}

void ImguiRenderer::GlfwMouseButtonCallbacks(GLFWwindow * window, int button, int action, int mods)
{
    if (action == GLFW_PRESS && button >= 0 && button < 3)
    {
        isMousePress[button] = true;
    }
}

void ImguiRenderer::GlfwScrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    mouseWheelPos += (float)yoffset;
}
