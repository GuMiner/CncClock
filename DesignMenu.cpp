#include <imgui\imgui.h>
#include "Gear.h"
#include "Indicator.h"
#include "DesignMenu.h"

DesignMenu::DesignMenu(Clock* clock)
    : clock(clock)
{
}

void DesignMenu::Update(b2World* world)
{
    if (ImGui::Begin("Design", nullptr, ImVec2(100, 100), 0.50f, ImGuiWindowFlags_MenuBar))
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Add"))
            {
                if (ImGui::MenuItem("Indicator"))
                {
                    clock->AddPart(new Indicator(world, clock->GetInsertPos()));
                }
                
                if (ImGui::MenuItem("Gear"))
                {
                    clock->AddPart(new Gear());
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }
    
    ImGui::End();
}
