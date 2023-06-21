#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <cstdint>
#include <string_view>

typedef uint8_t byte;

typedef uint16_t word;

// typedef uint16_t uint13_t; // shhh

class Emulator
{
public:

    // Program Counter
    word PC;

    // Accumulator
    byte AC;

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

    byte ROM[0x1000];

    Emulator();

    ~Emulator();

    void Reset();

    void LoadCartridge(std::string_view filename);

    void Tick();

    void PushWord(word data);

    word PopWord();

    byte ReadByte(word address);

    void WriteByte(word address, byte data);

    inline byte ReadWord(word address) {
        byte lower = ReadByte(address);
        byte upper = ReadByte(address + 1);
        return (upper << 8) | lower;
    }

    inline void WriteWord(word address, word data) {
        WriteByte(address, data & 0x00FF);
        WriteByte(address + 1, (data & 0xFF00) >> 8);
    }

};

#endif // EMULATOR_HPP