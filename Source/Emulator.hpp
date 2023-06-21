#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <cstdint>

typedef uint8_t byte;

typedef uint16_t word;

// typedef uint16_t uint13_t; // shhh

class Emulator
{
public:

    // Program Counter
    word PC;

    // Accumulator
    byte A;

    // X Register
    byte X;

    // Y Register
    byte Y;

    // Status Register
    union {
        struct {
            // Negative
            byte N : 1;

            // Overflow
            byte V : 1;
            
            byte   : 2;

            // Decimal
            byte D : 1;

            // Interrupt Disable
            byte I : 1;

            // Zero
            byte Z : 1;

            // Carry
            byte C : 1;
        };

        byte SR;
    };

    // Stack Pointer
    byte SP;

    byte RAM[0x80];

    unsigned ROMBank = 0;

    byte ROM[8][0x1000];

    byte EXTRAM[0x100];

    Emulator();

    ~Emulator();

    void Reset();

    void LoadCartridge(const char * filename);

    bool Tick();

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
        WriteByte(SP, data);
        SP -= 1;
    }

    inline byte PopByte() {
        SP += 1;
        return ReadByte(SP);
    }

    inline void PushWord(word data) {
        WriteWord(SP, data);
        SP -= 2;
    }

    inline word PopWord() {
        SP += 2;
        return ReadWord(SP);
    }

};

#endif // EMULATOR_HPP