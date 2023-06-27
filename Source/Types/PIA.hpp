#ifndef TYPES_PIA_HPP
#define TYPES_PIA_HPP

#include <Config.hpp>

union ConsoleSwitches
{
    struct {
        byte ResetUp : 1;
        byte SelectUp : 1;
        byte : 1;
        byte ColorEnabled : 1;
        byte : 2;
        byte P0DIFF : 1;
        byte P1DIFF : 1;
    };

    byte _raw;
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