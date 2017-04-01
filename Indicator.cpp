#include <imgui\imgui.h>
#include "Indicator.h"

Indicator::Indicator(b2World* world, glm::vec2 createPos)
    : length(8.0f), thickness(0.30f), angle(0.0f), indicatorRadius(0.05f), IPart()
{
    fixtures.push_back(new b2PolygonShape()); // Main body
    fixtures.push_back(new b2PolygonShape()); // Tip

    gCodeFixtures.push_back(new b2CircleShape()); // Indicator punch-thru hole
    
    RecreateBody(world, createPos);
}

void Indicator::RecreateBody(b2World* world, glm::vec2 createPos)
{
    // Main indicator
    ((b2PolygonShape*)fixtures[0])->SetAsBox(length / 2.0f, thickness / 2.0f);

    // CCW winding, indicator tip
    const int count = 3;
    b2Vec2 vertices[count];
    vertices[0].Set(length / 2.0f, -thickness);
    vertices[1].Set(length / 2.0f + thickness, 0.0f);
    vertices[2].Set(length / 2.0f, thickness);
    ((b2PolygonShape*)fixtures[1])->Set(&vertices[0], count);

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

    // Indicator punch to hold the indicator in place
    ((b2CircleShape*)gCodeFixtures[0])->m_radius = indicatorRadius;

    if (gCodeBody != nullptr)
    {
        world->DestroyBody(gCodeBody);
    }

    bodyDef.type = b2BodyType::b2_dynamicBody;
    bodyDef.position = b2Vec2(createPos.x, createPos.y);
    bodyDef.active = false;
    bodyDef.awake = false;
    bodyDef.angle = 0;
    bodyDef.gravityScale = 0.0f;

    gCodeBody = world->CreateBody(&bodyDef);
    for (b2Shape* shape : gCodeFixtures)
    {
        gCodeBody->CreateFixture(shape, 0.12f); // TODO revisit densities
    }

    gCodeBody->SetTransform(gCodeBody->GetTransform().p, angle * 3.1415926535f / 180.0f);
}

void Indicator::UpdateUi(b2World* world)
{
    if (ImGui::Begin("Indicator", nullptr, ImVec2(100, 100), 0.50f))
    {
        ImGui::SetCursorPos(ImVec2(5, 20));
        if (ImGui::SliderFloat("Thickness", &thickness, 0.1f, 10.0f) ||
            ImGui::SliderFloat("Length", &length, 0.1f, 10.0f) ||
            ImGui::SliderFloat("Angle", &angle, 0.0f, 360.0f) ||
            ImGui::SliderFloat("IndicatorRadius", &indicatorRadius, 0.05f, thickness / 2.0f))
        {
            RecreateBody(world, glm::vec2(body->GetPosition().x, body->GetPosition().y));
        }
    }
    ImGui::End();
}

void Indicator::MovePart(glm::vec2 pos)
{
    float bodyAngle = body->GetAngle();
    body->SetTransform(b2Vec2(pos.x, pos.y), bodyAngle);
    gCodeBody->SetTransform(b2Vec2(pos.x, pos.y), bodyAngle);
}
