#pragma once
#include <Box2D\Box2D.h>
#include <glm\vec2.hpp>
#include "IPart.h"

class Indicator : public IPart
{
    float length;
    float thickness;

public:
    Indicator(b2World* world, glm::vec2 createPos);

    // Inherited via IPart
    virtual void UpdateUi() override;
};

