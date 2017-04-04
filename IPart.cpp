#include "IPart.h"

IPart::IPart()
    : fixtures(), body(nullptr), gCodeBody(nullptr)
{
}

void IPart::RecreateBodyWithFixtures(b2World* world, glm::vec2 createPos, float angle)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyDef.position = b2Vec2(createPos.x, createPos.y);
    bodyDef.gravityScale = 0.0f;

    if (body != nullptr)
    {
        world->DestroyBody(body);
    }

    body = world->CreateBody(&bodyDef);
    for (b2Shape* shape : fixtures)
    {
        body->CreateFixture(shape, 0.12f); // TODO revisit densities
    }

    body->SetTransform(body->GetTransform().p, angle * 3.1415926535f / 180.0f);
}

void IPart::RecreateGCodeBodyWithFixtures(b2World* world, glm::vec2 createPos, float angle)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyDef.position = b2Vec2(createPos.x, createPos.y);
    bodyDef.active = false;
    bodyDef.awake = false;
    bodyDef.angle = 0;
    bodyDef.gravityScale = 0.0f;

    if (gCodeBody != nullptr)
    {
        world->DestroyBody(gCodeBody);
    }

    gCodeBody = world->CreateBody(&bodyDef);
    for (b2Shape* shape : gCodeFixtures)
    {
        gCodeBody->CreateFixture(shape, 0.12f); // TODO revisit densities
    }

    gCodeBody->SetTransform(gCodeBody->GetTransform().p, angle * 3.1415926535f / 180.0f);
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
