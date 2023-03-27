#pragma once

#include "drivers/tiramisu/ball_sorter/ball_sorter.hpp"
#include "drivers/navigation/tiramisu/tinav.hpp"
#include "drivers/tiramisu/arm/arm.hpp"
#include <kipr/analog/analog.hpp>


namespace go
{
    extern kipr::analog::Analog *line;
    extern kipr::analog::Analog *dist;
    extern BallSorter *balls;
    extern TINav *nav;
    extern Arm *arm;
}
