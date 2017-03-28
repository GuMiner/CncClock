#pragma once
#include "IPart.h"

class Gear : public IPart
{
public:
    Gear();

    // Inherited via IPart
    virtual void UpdateUi(b2World * world) override;
    virtual void MovePart(glm::vec2 pos) override;
};

