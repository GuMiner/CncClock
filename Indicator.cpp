#include "Indicator.h"

Indicator::Indicator(b2World* world, glm::vec2 createPos)
    : IPart()
{
    b2PolygonShape* mainIndicatorBody = new b2PolygonShape();
    mainIndicatorBody->SetAsBox(1.0f, 1.0f);
    
    fixtures.push_back(mainIndicatorBody);
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = b2Vec2(createPos.x, createPos.y);
    body = world->CreateBody(&bodyDef);
    body->CreateFixture(mainIndicatorBody, 0.12f); // Not really sure what to use here.
}

void Indicator::UpdateUi()
{
}
