#pragma once
#include <Box2D\Box2D.h>
#include <glm\vec2.hpp>
#include "IPart.h"

class Indicator : public IPart
{
    float angle;
    float length;
    float thickness;
    float indicatorRadius;

    void RecreateBody(b2World* world, glm::vec2 createPos);

public:
    Indicator(b2World* world, glm::vec2 createPos);

    // Inherited via IPart
    virtual void UpdateUi(b2World* world) override;
    virtual void MovePart(glm::vec2 pos) override;
};

