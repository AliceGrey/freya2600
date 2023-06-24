#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include "Constants.hpp"

#include <cstdint>

#include <SDL.h>

typedef uint8_t byte;

typedef uint16_t word;

// typedef uint16_t uint13_t; // shhh

union NUSIZ_t {
    struct {
        byte PSIZEa : 1;
        byte PSIZEb : 1;
        byte PSIZEc : 1;
        byte : 1;
        byte MSIZEa : 1;
        byte MSIZEb : 1;
        byte : 2;
    };
    byte _raw;
};

union COLUP_t {
    struct {
        byte : 1;
        byte LUM0 : 1;
        byte LUM1 : 1;
        byte LUM2 : 1;
        byte COL0 : 1;
        byte COL1 : 1;
        byte COL2 : 1;
        byte COL3 : 1;
    };
    struct {
        byte : 1;
        byte Index : 1;
    };
    byte _raw;
};

union REFP_t {
    struct {
        byte : 3;
        byte Enabled : 1;
        byte : 4;
    };
    byte _raw;
};

union AUDC_t {
    struct {
        byte D0: 1;
        byte D1: 1;
        byte D2: 1;
        byte D3: 1;
        byte : 4;
    };
    byte _raw;
};

union AUDF_t {
    struct {
        byte D0: 1;
        byte D1: 1;
        byte D2: 1;
        byte D3: 1;
        byte D4: 1;
        byte : 3;
    };
    byte _raw;
};

union AUDV_t {
    struct {
        byte D0: 1;
        byte D1: 1;
        byte D2: 1;
        byte D3: 1;
        byte : 4;
    };
    byte _raw;
};

union GRP_t {
    struct {
        byte Dot0Enabled : 1;
        byte Dot1Enabled : 1;
        byte Dot2Enabled : 1;
        byte Dot3Enabled : 1;
        byte Dot4Enabled : 1;
        byte Dot5Enabled : 1;
        byte Dot6Enabled : 1;
        byte Dot7Enabled : 1;
    };
    byte _raw;
};

union ENA_t {
    struct {
        byte : 1;
        byte Enabled : 1;
        byte : 6;
    };
    byte _raw;
};

union HM_t {
    struct {
        byte : 4;
        byte D4: 1;
        byte D5: 1;
        byte D6: 1;
        byte D7: 1;
        
    };
    byte _raw;
};

union VDEL_t {
    struct {
        byte Enabled : 1;
        byte : 7;
    };
    byte _raw;
};

union RESMP_t {
    struct {
        byte: 1;
        byte Reset: 1;
        byte : 6;
    };
    byte _raw;
};


class Emulator
{
public:

    // Program Counter
    word PC;

    // Stack Pointer
    byte SP;

    // Accumulator
    byte A;

    // X Register
    byte X;

    // Y Register
    byte Y;

    // Status Register
    union {
        struct {
            // Carry
            byte C : 1;

            // Zero
            byte Z : 1;

            // Interrupt Disable
            byte I : 1;

            // Decimal
            byte D : 1;
            
            byte   : 2;

            // Overflow
            byte V : 1;

            // Negative
            byte N : 1;
        };

        byte SR;
    };

    union {
        struct {
            byte : 1;
            byte Enabled : 1;
            byte : 6;
        };
        byte _raw;
    } VSYNC;

    union {
        struct {
            byte : 1;
            byte Enabled : 1;
            byte : 4;
            byte LatchEnabled : 1;
            byte GroundEnabled : 1;
        };
        byte _raw;
    } VBLANK;

    NUSIZ_t NUSIZ0;

    NUSIZ_t NUSIZ1;

    COLUP_t COLUP0;

    COLUP_t COLUP1;

    COLUP_t COLUPF;

    COLUP_t COLUBK;

    GRP_t GRP0;

    GRP_t GRP1;

    union {
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
    } CTRLPF;

    REFP_t REFP0;

    REFP_t REFP1;

    byte PF[3];

    AUDC_t AUDC0;

    AUDC_t AUDC1;

    AUDF_t AUDF0;

    AUDF_t AUDF1;

    AUDV_t AUDV0;
    
    AUDV_t AUDV1;

    ENA_t ENAM0; // Enable Missile 0

    ENA_t ENAM1; // Enable Missile 1

    ENA_t ENABL; // Enable Ball

    HM_t HMP0; // Horizontal Motion Player 0

    HM_t HMP1; // Horizontal Motion Player 1

    HM_t HMM0; // Horizontal Motion Missile 0

    HM_t HMM1; // Horizontal Motion Missile 1

    HM_t HMBL; // Horizontal Motion Ball 1

    VDEL_t VDELP0; // Vertical Delay Player 0
    
    VDEL_t VDELP1; // Vertical Delay Player 1
    
    VDEL_t VDELBL; // Vertical Delay Ball 1

    RESMP_t RESMP0; // Reset Missile Player 0
    
    RESMP_t RESMP1; // Reset Missile Player 1

    bool WSYNC = false;



    byte RAM[0x80];

    unsigned ROMBank = 0;

    byte ROM[8][0x1000];

    byte EXTRAM[0x100];

    SDL_Window * Window = nullptr;

    SDL_Point WindowSize = {
        DISPLAY_WIDTH * 2,
        DISPLAY_HEIGHT * 2,
    };

    SDL_Renderer * Renderer = nullptr;

    SDL_Texture * ScreenTexture = nullptr;

    uint8_t ScreenBuffer[SCREEN_BUFFER_SIZE];

    unsigned MemoryLine = 0;

    PPUState DrawState;

    Emulator();

    ~Emulator();

    void Run();

    void Reset();

    void LoadCartridge(const char * filename);

    void TickCPU();

    void TickTIA();

    byte ReadByte(word address);

    void WriteByte(word address, byte data);

    inline byte NextByte() {
        return ReadByte(PC++);
    }

    inline word NextWord() {
        byte lower = NextByte();
        byte upper = NextByte();
        return (upper << 8) | lower;
    }

    inline word ReadWord(word address) {
        byte lower = ReadByte(address);
        byte upper = ReadByte(address + 1);
        return (upper << 8) | lower;
    }

    inline void WriteWord(word address, word data) {
        WriteByte(address, data & 0x00FF);
        WriteByte(address + 1, (data & 0xFF00) >> 8);
    }

    inline void PushByte(byte data) {
        SP -= 1;
        WriteByte(SP, data);
    }

    inline byte PopByte() {
        byte data = ReadByte(SP);
        SP += 1;
        return data;
    }

    inline void PushWord(word data) {
        SP -= 2;
        WriteWord(SP, data);
    }

    inline word PopWord() {
        word data = ReadWord(SP);
        SP += 2;
        return data;
    }

};

#endif // EMULATOR_HPP