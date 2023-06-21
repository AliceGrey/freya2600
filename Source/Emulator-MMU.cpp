#include "Emulator.hpp"

uint8_t Emulator::ReadByte(word address)
{
    if (address >= 0x0080 && address <= 0x00FF) {
        return RAM[address - 0x0080];
    }

    // MAX GO AWAY STOP ATTACKING ME

    return 0;
}

void Emulator::WriteByte(word address, byte data)
{
    if (address >= 0x0080 && address <= 0x00FF) {
        RAM[address - 0x0080] = data;
    }
}
