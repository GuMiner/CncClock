#include <imgui\imgui.h>
#include "logging\Logger.h"
#include "Input.h"
#include "Clock.h"


Clock::Clock()
    : parts(), insertPos(0.0f, 0.0f), selectedPart(-1), leftPressHandled(false), rightPressHandled(false)
{
}

glm::vec2 Clock::GetInsertPos() const
{
    return insertPos;
}

void Clock::AddPart(IPart* part)
{
    parts.push_back(part);
    Logger::Log("Added clock part for a total of ", parts.size(), " parts.");
}

void Clock::Update(b2World* world, Viewer* viewer)
{
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        // Select a part or select where a part will be created.
        if (!leftPressHandled && Input::IsMouseButtonPressed(0))
        {
            glm::vec2 gridPos = viewer->GetGridPos(Input::GetMousePos());
            Logger::Log("Clicked ", gridPos);

            selectedPart = -1;
            for (unsigned int i = 0; i < parts.size(); i++)
            {
                if (parts[i]->TestPoint(gridPos))
                {
                    Logger::Log("Selected part ", i, " of ", parts.size(), " parts.");
                    selectedPart = i;
                    break;
                }
            }

            if (selectedPart == -1)
            {
                Logger::Log("Unselected part, new part creation will be at ", gridPos);
                insertPos = gridPos;
            }

            leftPressHandled = true;
        }
        else if (!Input::IsMouseButtonPressed(0))
        {
            leftPressHandled = false;
        }

        // Moves a part.
        if (!rightPressHandled && Input::IsMouseButtonPressed(1))
        {
            if (selectedPart != -1)
            {
                glm::vec2 gridPos = viewer->GetGridPos(Input::GetMousePos());
                Logger::Log("Moving part ", selectedPart, " to ", gridPos);

                parts[selectedPart]->MovePart(gridPos);
            }

            rightPressHandled = true;
        }
        else if (!Input::IsMouseButtonPressed(1))
        {
            rightPressHandled = false;
        }
    }

    if (selectedPart != -1)
    {
        parts[selectedPart]->UpdateUi(world);
    }
}

Clock::~Clock()
{
}
