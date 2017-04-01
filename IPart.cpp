#include "IPart.h"

IPart::IPart()
    : fixtures(), body(nullptr), gCodeBody(nullptr)
{
}

bool IPart::TestPoint(glm::vec2 pos)
{
    if (body != nullptr)
    {
        for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext())
        {
            if (fixture->TestPoint(b2Vec2(pos.x, pos.y)))
            {
                return true;
            }
        }
    }

    return false;
}
