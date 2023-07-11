#ifndef DISASSEMBLY_HPP
#define DISASSEMBLY_HPP

#include <Config.hpp>

class Emulator;

struct InstructionDefinition
{
    unsigned ByteCount;

    const char * Format;

}; // struct InstructionDefinition

struct InstructionRecord
{
    word Address;

    unsigned ROMBank = 0;

    bool JumpDestination = false;

    byte Opcodes[3];

    const InstructionDefinition * Definition;

    InstructionRecord * Prev = nullptr;

    InstructionRecord * Next = nullptr;

    InstructionRecord(Emulator * emu, word address);
    
    const char * ToString();

}; // struct InstructionRecord

#endif // DISASSEMBLY_HPP