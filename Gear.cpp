#include <imgui\imgui.h>
#include "Gear.h"

Gear::Gear(b2World* world, glm::vec2 createPos)
    : radius(5.0f), generatingCircleRadius(0.25f), centerRadius(0.50f), 
      teeth(20), divisionsPerHalfTooth(2), isPinion(false), angle(0.0f), IPart()
{
    RecreateBody(world, createPos);
}

void Gear::RecreateBody(b2World* world, glm::vec2 createPos)
{
    fixtures.clear();
    gCodeFixtures.clear();

    fixtures.push_back(new b2CircleShape()); // Main body.
    ((b2CircleShape*)fixtures[0])->m_radius = radius;

    // Gear teeth
    float distanceIncrement = (3.1415926535f * radius) / (teeth * 2 * divisionsPerHalfTooth);
    float anglePerTooth = 2 * 3.141592653589f / teeth;
    float anglePerDistanceIncrement = anglePerTooth / (2.0f * divisionsPerHalfTooth * 2.0f);
    for (int i = 0; i < teeth; i++)
    {
        for (int j = 0; j < divisionsPerHalfTooth; j++)
        {
            // TODO this only renders half the tooth -- and the profile is wrong -- but we're getting close.
            float startDivHeight = GetCycloidHeight((float)j * distanceIncrement, generatingCircleRadius);
            float nextDivHeight = GetCycloidHeight((float)(j + 1) * distanceIncrement, generatingCircleRadius);

            float leftPosAngle = angle + anglePerDistanceIncrement * (float)j + anglePerTooth * (float)i;
            float rightPosAngle = anglePerDistanceIncrement + leftPosAngle;
            glm::vec2 divisionLeftPos = glm::vec2(radius * std::cos(leftPosAngle), radius * std::sin(leftPosAngle));
            glm::vec2 divisionRightPos = glm::vec2(radius * std::cos(rightPosAngle), radius * std::sin(rightPosAngle));

            const int count = 4; // CCW winding
            b2PolygonShape* tooth = new b2PolygonShape();
            b2Vec2 vertices[count];
            vertices[0].Set(divisionLeftPos.x, divisionLeftPos.y);
            vertices[1].Set(divisionLeftPos.x * (startDivHeight + radius) / radius, divisionLeftPos.y * (startDivHeight + radius) / radius);
            vertices[2].Set(divisionRightPos.x * (nextDivHeight + radius) / radius, divisionRightPos.y * (nextDivHeight + radius) / radius);
            vertices[3].Set(divisionRightPos.x, divisionRightPos.y);
            tooth->Set(&vertices[0], count);

            fixtures.push_back(tooth);
        }
    }

    gCodeFixtures.push_back(new b2CircleShape()); // Indicator punch-thru hole
    ((b2CircleShape*)gCodeFixtures[0])->m_radius = centerRadius;

    RecreateBodyWithFixtures(world, createPos, angle);
    RecreateGCodeBodyWithFixtures(world, createPos, angle);
}

float Gear::GetCycloidHeight(float dist, float cycloidRadius)
{
    return (1 - std::sin((3.141592653589f / 2.0f) - (dist / cycloidRadius))) * cycloidRadius;
}

void Gear::UpdateUi(b2World* world)
{
    if (ImGui::Begin("Gear", nullptr, ImVec2(100, 100), 0.50f))
    {
        ImGui::SetCursorPos(ImVec2(5, 20));
        if (ImGui::SliderFloat("Radius", &radius, 0.1f, 20.0f) ||
            ImGui::SliderFloat("Gen-Cir Radius", &generatingCircleRadius, 0.01f, radius / 2.0f) ||
            ImGui::SliderFloat("Center Radius", &centerRadius, 0.01f, radius) ||
            ImGui::SliderInt("Teeth", &teeth, 2, 1000) ||
            ImGui::SliderInt("Div-Per-Half-Tooth", &divisionsPerHalfTooth, 1, 20) || 
            ImGui::Checkbox("IsPinion", &isPinion) ||
            ImGui::SliderFloat("Angle", &angle, 0.0f, 360.0f))
        {
            RecreateBody(world, glm::vec2(body->GetPosition().x, body->GetPosition().y));
        }
    }
    ImGui::End();

    // TODO interesting test cod.e
    body->ApplyTorque(20.0f, true);
}

void Gear::MovePart(glm::vec2 pos)
{
    float bodyAngle = body->GetAngle();
    body->SetTransform(b2Vec2(pos.x, pos.y), bodyAngle);
    gCodeBody->SetTransform(b2Vec2(pos.x, pos.y), bodyAngle);
    
    // Reset motion on moving.
    body->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
    body->SetAngularVelocity(0.0f);
}
