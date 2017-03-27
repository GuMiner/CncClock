#include "logging\Logger.h"
#include "LineRenderer.h"

LineRendererProgram LineRenderer::glslProgram;

bool LineRenderer::LoadProgram(ShaderFactory* shaderFactory)
{
    Logger::Log("Loading the point rendering shading program for line rendering...");
    if (!shaderFactory->CreateShaderProgram("lineRender", &glslProgram.programId))
    {
        Logger::LogError("Failed to load the line rendering shader; cannot continue.");
        return false;
    }

    glslProgram.projMatrixLocation = glGetUniformLocation(glslProgram.programId, "projMatrix");
    glslProgram.transparencyLocation = glGetUniformLocation(glslProgram.programId, "transparency");

    return true;
}

LineRenderer::LineRenderer(bool isLineStrip)
    : isLineStrip(isLineStrip)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    positionBuffer.Initialize();
    colorBuffer.Initialize();
    lastBufferSize = 0;
}

void LineRenderer::Update()
{
    glBindVertexArray(vao);
    positionBuffer.TransferToOpenGl();
    colorBuffer.TransferToOpenGl();
    lastBufferSize = positionBuffer.vertices.size();
}


void LineRenderer::Render(glm::mat4& projectionMatrix)
{
    glUseProgram(glslProgram.programId);
    glBindVertexArray(vao);
    glUniformMatrix4fv(glslProgram.projMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    // TODO should be configurable.
    glUniform1f(glslProgram.transparencyLocation, 1.0f);

    glDrawArrays(isLineStrip ? GL_LINE_STRIP : GL_LINES, 0, lastBufferSize);
}

void LineRenderer::Clear()
{
    positionBuffer.vertices.clear();
    colorBuffer.vertices.clear();
}

void LineRenderer::AddXYRectangle(glm::vec3 lowerRightPos, glm::vec2 size, glm::vec3 color)
{
    positionBuffer.vertices.push_back(lowerRightPos);
    positionBuffer.vertices.push_back(lowerRightPos + glm::vec3(size.x, 0.0f, 0.0f));

    positionBuffer.vertices.push_back(lowerRightPos + glm::vec3(size.x, 0.0f, 0.0f));
    positionBuffer.vertices.push_back(lowerRightPos + glm::vec3(size.x, size.y, 0.0f));

    positionBuffer.vertices.push_back(lowerRightPos + glm::vec3(size.x, size.y, 0.0f));
    positionBuffer.vertices.push_back(lowerRightPos + glm::vec3(0, size.y, 0.0f));

    positionBuffer.vertices.push_back(lowerRightPos + glm::vec3(0, size.y, 0.0f));
    positionBuffer.vertices.push_back(lowerRightPos);

    for (int i = 0; i < 8; i++)
    {
        colorBuffer.vertices.push_back(color);
    }
}

LineRenderer::~LineRenderer()
{
    glDeleteVertexArrays(1, &vao);
    positionBuffer.Deinitialize();
    colorBuffer.Deinitialize();
}
