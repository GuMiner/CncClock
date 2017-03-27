#pragma once
#include <GL/glew.h>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <glm\mat4x4.hpp>
#include "GuCommon\shaders\ShaderFactory.h"
#include "GuCommon\vertex\PositionVbo.hpp"
#include "GuCommon\vertex\ColorVbo.hpp"

struct LineRendererProgram
{
    GLuint programId;

    GLuint projMatrixLocation;
    GLuint transparencyLocation;
};

// Renders lines.
class LineRenderer
{
    static LineRendererProgram glslProgram;

    GLuint vao;
    std::size_t lastBufferSize;

    bool isLineStrip;

public:
    static bool LoadProgram(ShaderFactory* shaderFactory);

    PositionVbo positionBuffer;
    ColorVbo colorBuffer;

    LineRenderer(bool isLineStrip);

    // Updates the GPU with whatever is currently in the line buffers.
    void Update();

    // Renders the lines in the provided buffers.
    void Render(glm::mat4& projectionMatrix);

    // Helper methods for dealing with lines.
    void Clear();
    void AddXYRectangle(glm::vec3 lowerRightPos, glm::vec2 size, glm::vec3 color);


    ~LineRenderer();
};

