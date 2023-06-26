#ifndef TYPES_TIA_HPP
#define TYPES_TIA_HPP

#include <Config.hpp>

union VerticalSync
{
    struct {
        byte : 1;
        byte Enabled : 1;
        byte : 6;
    };

    byte _raw;
};

static_assert(
    sizeof(VerticalSync) == sizeof(VerticalSync::_raw)
);

union VerticalBlank
{
    struct {
        byte : 1;
        byte Enabled : 1;
        byte : 4;
        byte LatchEnabled : 1;
        byte GroundEnabled : 1;
    };

    byte _raw;
};

static_assert(
    sizeof(VerticalBlank) == sizeof(VerticalBlank::_raw)
);

union ColorPicker
{
    struct {
        byte : 1;
        byte LUM : 3;
        byte COL : 4;
    };

    struct {
        byte : 1;
        byte Index : 7;
    };

    byte _raw;   
};

static_assert(
    sizeof(ColorPicker) == sizeof(ColorPicker::_raw)
);

union PlayerSizeCount
{
    struct {
        byte PSIZE : 3;
        byte : 1;
        byte MSIZE : 2;
        byte : 2;
    };

    byte _raw;
};

static_assert(
    sizeof(PlayerSizeCount) == sizeof(PlayerSizeCount::_raw)
);

union PlayerFieldControl
{
    struct {
        byte ReflectEnabled : 1;
        byte ScoreColorMode : 1;
        byte Priority: 1;
        byte : 1;
        byte BSIZE0 : 1;
        byte BSIZE1 : 1;
        byte : 2;
    };

    byte _raw;
};

static_assert(
    sizeof(PlayerFieldControl) == sizeof(PlayerFieldControl::_raw)
);

union PlayerReflect
{
    struct {
        byte : 3;
        byte Enabled : 1;
        byte : 4;
    };

    byte _raw;
};

static_assert(
    sizeof(PlayerReflect) == sizeof(PlayerReflect::_raw)
);

union AudioControl
{
    struct {
        byte Mode: 4;
        byte : 4;
    };

    byte _raw;
};

static_assert(
    sizeof(AudioControl) == sizeof(AudioControl::_raw)
);

union AudioFrequencyDivide
{
    struct {
        byte Amount : 5;
        byte : 3;
    };

    byte _raw;
};

static_assert(
    sizeof(AudioFrequencyDivide) == sizeof(AudioFrequencyDivide::_raw)
);

union AudioVolume
{
    struct {
        byte Volume : 4;
        byte : 4;
    };

    byte _raw;
};

static_assert(
    sizeof(AudioVolume) == sizeof(AudioVolume::_raw)
);

union HorizontalMotion
{
    struct {
        byte : 4;
        int8_t Amount : 4; // TODO: Test
    };

    byte _raw;
};

static_assert(
    sizeof(HorizontalMotion) == sizeof(HorizontalMotion::_raw)
);

union VerticalDelay
{
    struct {
        byte Enabled : 1;
        byte : 7;
    };

    byte _raw;
};

static_assert(
    sizeof(VerticalDelay) == sizeof(VerticalDelay::_raw)
);

union BallMissileEnable
{
    struct {
        byte : 1;
        byte Enabled : 1;
        byte : 6;
    };

    byte _raw;
};

static_assert(
    sizeof(BallMissileEnable) == sizeof(BallMissileEnable::_raw)
);

union MissileReset
{
    struct {
        byte: 1;
        byte Reset: 1;
        byte : 6;
    };

    byte _raw;
};

static_assert(
    sizeof(MissileReset) == sizeof(MissileReset::_raw)
);

#endif // TYPES_TIA_HPP