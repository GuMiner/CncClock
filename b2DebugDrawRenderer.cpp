#include "b2DebugDrawRenderer.h"
const float PI = 3.1415926535f;

b2DebugDrawRenderer::b2DebugDrawRenderer() : lineRenderer(false)
{
}

void b2DebugDrawRenderer::AddPoint(const b2Vec2& pos, const b2Color& color)
{
    lineRenderer.positionBuffer.vertices.push_back(glm::vec3(pos.x, pos.y, 0.0f));
    lineRenderer.colorBuffer.vertices.push_back(glm::vec3(color.r, color.g, color.b));
}

void b2DebugDrawRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    for (int i = 0; i < vertexCount; i++)
    {
        int nextVertexId = i == vertexCount - 1 ? 0 : i + 1;
        AddPoint(vertices[i], color);
        AddPoint(vertices[nextVertexId], color);
    }
}

void b2DebugDrawRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
    // TODO fill in.
    DrawPolygon(vertices, vertexCount, color);
}

void b2DebugDrawRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
    int resolution = 32;
    for (int i = 0; i < resolution; i++)
    {
        int nextVertexId = i == resolution - 1 ? 0 : i + 1;
        float pointAngle = (2.0f * PI * (float)i) / (float)resolution;
        float nextPointAngle = (2.0f * PI * (float)nextVertexId) / (float)resolution;
        b2Vec2 pos = center + (radius * b2Vec2(std::cos(pointAngle), std::sin(pointAngle)));
        b2Vec2 nextPos = center + (radius * b2Vec2(std::cos(nextPointAngle), std::sin(nextPointAngle)));

        AddPoint(pos, color);
        AddPoint(nextPos, color);
    }
}

void b2DebugDrawRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
    // TODO fill in.
    DrawSolidCircle(center, radius, axis, color);
}

void b2DebugDrawRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
    AddPoint(p1, color);
    AddPoint(p2, color);
}

void b2DebugDrawRenderer::DrawTransform(const b2Transform & xf)
{
    // ?
}

void b2DebugDrawRenderer::DrawPoint(const b2Vec2& p, float32 size, const b2Color & color)
{
    AddPoint(p - b2Vec2(size, size), color);
    AddPoint(p + b2Vec2(size, size), color);

    AddPoint(p + b2Vec2(-size, size), color);
    AddPoint(p + b2Vec2(size, -size), color);
}

void b2DebugDrawRenderer::Update()
{
    lineRenderer.Update();
    lineRenderer.Clear();
}

void b2DebugDrawRenderer::Render(glm::mat4& projectionMatrix)
{
    lineRenderer.Render(projectionMatrix);
}
