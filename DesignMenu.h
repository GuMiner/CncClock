#pragma once
#include "Clock.h"

class DesignMenu
{
    Clock* clock;

public:
    DesignMenu(Clock* clock);
    void Update(b2World* world);
};

