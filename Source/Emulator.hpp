#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <Config.hpp>
#include <Constants.hpp>
#include <Types/CPU.hpp>
#include <Types/PIA.hpp>
#include <Types/TIA.hpp>

#include <SDL.h>

#include "Debugger.hpp"

class Emulator
{
public:

    ///
    /// CPU / MOS6507
    ///

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
            
            // B
            byte B : 1;

            byte   : 1;

            // Overflow
            byte V : 1;

            // Negative
            byte N : 1;
        };

        byte SR;
    };

    word LastInstructionAddress;

    byte FetchCache[3];

    ///
    /// RAM I/O Timer / RIOT
    /// Peripheral Interface Adaptor / PIA
    ///

    byte RAM[0x80];

    // Port A I/O Data
    JoystickRegister SWCHA;

    // Port A I/O Direction
    byte SWACNT;

    // Port B I/O Data, hardwired to Console Switches
    ConsoleSwitches SWCHB;

    // Port B I/O Direction
    byte SWBCNT;

    // Timer Countdown
    byte INTIM;

    TimerInterrupt TIMINT; // Cowabunga

    // Number of cycles between ticks of the timer
    unsigned TimerInterval;

    // Number of cycles since the last tick of the timer
    unsigned TimerCounter;

    ///
    /// Television Interface Adaptor / TIA
    ///

    VerticalSync VSYNC;

    VerticalBlank VBLANK;

    PlayerSizeCount NUSIZ0;

    PlayerSizeCount NUSIZ1;

    ColorPicker COLUP0;

    ColorPicker COLUP1;

    ColorPicker COLUPF;

    ColorPicker COLUBK;

    byte GRP0; // Player 0 Graphics

    byte GRP1; // Player 1 Graphics

    unsigned SpriteCounterP0 = 0;
    unsigned SpriteCounterP1 = 0;

    PlayerFieldControl CTRLPF;

    PlayerReflect REFP0;

    PlayerReflect REFP1;

    byte PF[3];

    AudioControl AUDC0;

    AudioControl AUDC1;

    AudioFrequencyDivide AUDF0;

    AudioFrequencyDivide AUDF1;

    AudioVolume AUDV0;
    
    AudioVolume AUDV1;

    BallMissileEnable ENAM0; // Enable Missile 0

    BallMissileEnable ENAM1; // Enable Missile 1

    BallMissileEnable ENABL; // Enable Ball

    HorizontalMotion HMP0; // Horizontal Motion Player 0

    HorizontalMotion HMP1; // Horizontal Motion Player 1

    HorizontalMotion HMM0; // Horizontal Motion Missile 0

    HorizontalMotion HMM1; // Horizontal Motion Missile 1

    HorizontalMotion HMBL; // Horizontal Motion Ball 1

    VerticalDelay VDELP0; // Vertical Delay Player 0
    
    VerticalDelay VDELP1; // Vertical Delay Player 1
    
    VerticalDelay VDELBL; // Vertical Delay Ball 1

    MissileReset RESMP0; // Reset Missile Player 0
    
    MissileReset RESMP1; // Reset Missile Player 1

    // CPU is waiting for H-Blank
    bool WSYNC;

    uintmax_t LastWSYNC = 0;

    ///
    /// Cartridge
    ///

    byte ROM[MAX_BANKS][ROM_BANK_SIZE];

    int ROMBank = 0; // currently selected bank of ROM

    byte EXTRAM[0x100];
    
    bool IsPlaying;

    SDL_Window * Window = nullptr;

    unsigned WindowID;

    SDL_Point WindowSize = {
        DISPLAY_WIDTH * 2,
        DISPLAY_HEIGHT * 2,
    };

    SDL_Renderer * Renderer = nullptr;

    SDL_Texture * ScreenTexture = nullptr;

    uint8_t ScreenBuffer[SCREEN_BUFFER_SIZE];

    unsigned MemoryLine = 0;

    unsigned MemoryColumn = 0;

    uintmax_t CPUCycleCount = 0;

    uintmax_t TIACycleCount = 0;

    uintmax_t FrameCount = 0;

    Debugger * Debug = nullptr;

    FILE* tLog;

    Emulator();

    ~Emulator();

    void StartDebugger();

    void Reset();

    void LoadCartridge(const char * filename);

    void Run();

    void DoStep();

    void DoLine();

    void DoFrame();

    void TickCPU();

    void TickTIA();

    void TickPIA();

    byte ReadByte(word address, bool tick = true);

    void WriteByte(word address, byte data);

    // const char * Disassemble(word address);

    SDL_Color GetColor(uint8_t index);

    void printRAMGrid(const uint8_t* RAM);

    void printTraceLogHeaders(const char* filename);

    // void printRegisters();

    inline byte NextByte() {
        return ReadByte(PC++);
    }

    inline word NextWord() {
        byte lower = NextByte();
        byte upper = NextByte();
        return (upper << 8) | lower;
    }

    inline word ReadWord(word address, bool tick = true) {
        byte lower = ReadByte(address, tick);
        byte upper = ReadByte(address + 1, tick);
        return (upper << 8) | lower;
    }

    inline void WriteWord(word address, word data) {
        WriteByte(address, data & 0x00FF);
        WriteByte(address + 1, (data & 0xFF00) >> 8);
    }

    inline void PushByte(byte data) {
        SP -= 1;
        WriteByte(SP + 1, data);
    }

    inline byte PopByte() {
        byte data = ReadByte(SP + 1);
        SP += 1;
        return data;
    }

    inline void PushWord(word data) {
        SP -= 2;
        WriteWord(SP + 1, data);
    }

    inline word PopWord() {
        word data = ReadWord(SP + 1);
        SP += 2;
        return data;
    }

};

#endif // EMULATOR_HPP