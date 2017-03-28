#include <imgui\imgui.h>
#include "logging\Logger.h"
#include "Input.h"
#include "Clock.h"


Clock::Clock()
    : parts(), insertPos(0.0f, 0.0f), selectedPart(-1), pressHandled(false)
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

void Clock::Update(Viewer* viewer)
{
    if (!ImGui::GetIO().WantCaptureMouse)
    {
        if (!pressHandled && Input::IsMouseButtonPressed(0))
        {
            glm::vec2 mousePos = viewer->GetGridPos(Input::GetMousePos());

            selectedPart = -1;
            for (unsigned int i = 0; i < parts.size(); i++)
            {
                if (parts[i]->TestPoint(mousePos))
                {
                    Logger::Log("Selected part ", i, " of ", parts.size(), " parts.");
                    selectedPart = i;
                    break;
                }
            }

            if (selectedPart == -1)
            {
                Logger::Log("Unselected part, new part creation will be at ", mousePos);
                insertPos = mousePos;
            }

            pressHandled = true;
        }
        else if (!Input::IsMouseButtonPressed(0))
        {
            pressHandled = false;
        }
    }

    if (selectedPart != -1)
    {
        parts[selectedPart]->UpdateUi();
    }
}

Clock::~Clock()
{
}
