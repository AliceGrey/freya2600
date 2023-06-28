#ifndef TYPES_PIA_HPP
#define TYPES_PIA_HPP

#include <Config.hpp>

union ConsoleSwitches
{
    struct {
        byte Reset : 1; // 0 = Pressed, 1 = Released
        byte Select : 1; // 0 = Pressed, 1 = Released
        byte : 1;
        byte ColorEnabled : 1;
        byte : 2;
        byte P0DIFF : 1;
        byte P1DIFF : 1;
    };

    byte _raw; // Buttons are inverted yo
};

union JoystickRegister
{
    struct {
        byte P1Up: 1;
        byte P1Down : 1;
        byte P1Left : 1;
        byte P1Right: 1;
        byte P0Up: 1;
        byte P0Down : 1;
        byte P0Left: 1;
        byte P0Right: 1;
    };

    byte _raw; // Buttons are inverted yo
};

static_assert(
    sizeof(ConsoleSwitches) == sizeof(ConsoleSwitches::_raw)
);

union TimerInterrupt
{
    struct {
        byte Timer : 1;
        byte EdgeDetect : 1; // TODO: ???
        byte : 6;
    };

    byte _raw;
};

static_assert(
    sizeof(TimerInterrupt) == sizeof(TimerInterrupt::_raw)
);

#endif // TYPES_PIA_HPP