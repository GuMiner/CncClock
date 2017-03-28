#pragma once
#include "IPart.h"

class Gear : public IPart
{
public:
    Gear();

    // Inherited via IPart
    virtual void UpdateUi() override;
};

