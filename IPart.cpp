#include "IPart.h"
#include "Ground.h"

IPart::IPart(glm::vec2 createPos, float angle)
    : fixtures(), body(nullptr), gCodeBody(nullptr), fixtureJoint(nullptr), isFixed(false), currentPosition(createPos), angle(angle)
{
}

bool IPart::UpdateCommonUi()
{
    return (
        ImGui::SliderFloat("Angle", &angle, 0.0f, 360.0f) ||
        ImGui::InputFloat2("Position", &currentPosition[0], 3) || 
        ImGui::Checkbox("Fixed?", &isFixed));
}

void IPart::RecreateBodyWithFixtures(b2World* world)
{
    // Joints are destroyed when bodies are destroyed, so destroy the joint first.
    if (fixtureJoint != nullptr)
    {
        world->DestroyJoint(fixtureJoint);
        fixtureJoint = nullptr;
    }

    b2BodyDef bodyDef;
    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyDef.position = b2Vec2(currentPosition.x, currentPosition.y);
    bodyDef.gravityScale = 0.0f;
   
    if (body != nullptr)
    {
        world->DestroyBody(body);
        body = nullptr;
    }

    body = world->CreateBody(&bodyDef);
    for (b2Shape* shape : fixtures)
    {
        body->CreateFixture(shape, 0.12f); // TODO revisit densities
    }

    body->SetTransform(body->GetTransform().p, angle * 3.1415926535f / 180.0f);

    if (isFixed)
    {
        b2RevoluteJointDef fixedJoint;
        fixedJoint.bodyA = body;
        fixedJoint.bodyB = Ground::GetGround();
        fixedJoint.collideConnected = false;
        fixedJoint.localAnchorA = b2Vec2(0, 0);
        fixedJoint.localAnchorB = body->GetPosition() - Ground::GetGround()->GetPosition();

        fixtureJoint = (b2RevoluteJoint*)world->CreateJoint(&fixedJoint);
    }
}

void IPart::RecreateGCodeBodyWithFixtures(b2World* world)
{
    b2BodyDef bodyDef;
    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyDef.position = b2Vec2(currentPosition.x, currentPosition.y);
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
