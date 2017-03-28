#pragma once
#include <vector>
#include <glm\vec2.hpp>
#include "IPart.h"
#include "Viewer.h"

// The clock that is being designed
class Clock
{
    std::vector<IPart*> parts;
    
    glm::vec2 insertPos;
    unsigned int selectedPart;
    bool pressHandled;

public:
    Clock();
    
    glm::vec2 GetInsertPos() const;

    void AddPart(IPart* part);
    void Update(Viewer* viewer);
    ~Clock();
};

