#pragma once
#include <Box2D\Box2D.h>
#include <Box2D\Common\b2Draw.h>
#include "LineRenderer.h"

class b2DebugDrawRenderer : public b2Draw
{
    LineRenderer lineRenderer;

    // Adds a point into the line renderer.
    void AddPoint(const b2Vec2& pos, const b2Color& color);

public:
    b2DebugDrawRenderer();

    // Inherited via b2Draw
    virtual void DrawPolygon(const b2Vec2 * vertices, int32 vertexCount, const b2Color & color) override;
    virtual void DrawSolidPolygon(const b2Vec2 * vertices, int32 vertexCount, const b2Color & color) override;
    virtual void DrawCircle(const b2Vec2 & center, float32 radius, const b2Color & color) override;
    virtual void DrawSolidCircle(const b2Vec2 & center, float32 radius, const b2Vec2 & axis, const b2Color & color) override;
    virtual void DrawSegment(const b2Vec2 & p1, const b2Vec2 & p2, const b2Color & color) override;
    virtual void DrawTransform(const b2Transform & xf) override;
    virtual void DrawPoint(const b2Vec2 & p, float32 size, const b2Color & color) override;

    void Update();
    void Render(glm::mat4& projectionMatrix);
};

