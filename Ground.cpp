#include "Ground.h"

b2Body* Ground::ground;

void Ground::SetupGround(b2World * world)
{
    b2BodyDef groundBodyDef;
    groundBodyDef.type = b2BodyType::b2_staticBody;
    groundBodyDef.position = b2Vec2(0, -10.0f);

    b2PolygonShape groundBodyShape;
    groundBodyShape.SetAsBox(10.0f, 1.0f);

    ground = world->CreateBody(&groundBodyDef);
    ground->CreateFixture(&groundBodyShape, 1.0f);
}

b2Body * Ground::GetGround()
{
    return ground;
}
