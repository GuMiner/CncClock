#pragma once
#include <vector>
#include <Box2D\Box2D.h>
#include <imgui\imgui.h>
#include <glm\vec2.hpp>

// Defines the top-level part in the clock.
class IPart
{
protected:
    // Shapes used for physical simululation
    std::vector<b2Shape*> fixtures;
    b2Body* body;

    b2RevoluteJoint* fixtureJoint;

    // Shapes that only exist for g-code rendering.
    std::vector<b2Shape*> gCodeFixtures;
    b2Body* gCodeBody;

    bool isFixed;
    glm::vec2 currentPosition;
    float angle;

    void RecreateBodyWithFixtures(b2World* world);
    void RecreateGCodeBodyWithFixtures(b2World* world);

    // Updates UI that is common for all parts.
    bool UpdateCommonUi();

public:
    IPart(glm::vec2 createPos, float angle);

    // Returns true if the point is within the part, false otherwise.
    virtual bool TestPoint(glm::vec2 pos);
    
    virtual void MovePart(glm::vec2 pos) = 0;
    virtual void UpdateUi(b2World* world) = 0;
};