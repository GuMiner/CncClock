#pragma once
#include <Box2D\Box2D.h>
#include <glm\vec2.hpp>
#include "IPart.h"

class Gear : public IPart
{
    float angle;

    float radius;
    float generatingCircleRadius;
    float centerRadius;
    int teeth;
    int divisionsPerHalfTooth;
    bool isPinion;
    
    void RecreateBody(b2World* world, glm::vec2 createPos);

    float GetCycloidHeight(float dist, float cycloidRadius);

public:
    Gear(b2World* world, glm::vec2 createPos);

    // Inherited via IPart
    virtual void UpdateUi(b2World* world) override;
    virtual void MovePart(glm::vec2 pos) override;
};

