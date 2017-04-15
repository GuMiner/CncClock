#pragma once
#include <Box2D\Box2D.h>

// Defines the ground. Used in physical scenarios where you need to affix some Box2D item to another thing.
class Ground
{
    static b2Body* ground;

public:
    static void SetupGround(b2World* world);
    static b2Body* GetGround();
};

