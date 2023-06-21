#include "Emulator.hpp"

uint8_t Emulator::ReadByte(word address)
{
    address = (address & 0b0001'1111'1111'1111);

    // $00 - $7F TIA 
    // $

    // Normal RAM
    if (address >= 0x80 && address <= 0xFF) {
        return RAM[address - 0x80];
    }

    // MAX GO AWAY STOP ATTACKING ME
    // - SLW 2021

    // RAMR
    // if (address >= 0x1080 && address <= 0x11FF) {
    //     return EXTRAM[address - 0x1080];
    // }

    // $F000 to $F200 (vaguely the EXTRAM)
    // $F000 to $F0FF (writing to EXTRAM)
    // $F100 to $F1FF (reading to EXTRAM)

    // ROM
    if (address >= 0x1000 && address <= 0x1FFF) {
        return ROM[ROMBank][address - 0x1000];
    }

    //sTr0be BANK
    // The bank number is selected by reading from (or by writing any value to) specific addresses, the addresses and corresponding bank numbers are:
    // Size   Banks  FFF4 FFF5 FFF6 FFF7 FFF8 FFF9 FFFA FFFB
    // 2K,4K  1      -    -    -    -    -    -    -    -
    // 8K     2      -    -    -    -    0    1    -    -
    // 12K    3      -    -    -    -    0    1    2    -
    // 16K    4      -    -    0    1    2    3    -    -
    // 32K    8      0    1    2    3    4    5    6    7
    if (address >= 0x1200 && address <= 0x1FFF) {
        
        // Execute functions 
    }

    return 0;
}

void Emulator::WriteByte(word address, byte data)
{
    address = (address & 0b0001'1111'1111'1111);
    
    if (address >= 0x0080 && address <= 0x00FF) {
        RAM[address - 0x0080] = data;
    }

    // // RAMW
    // if (address >= 0x1000 && address <= 0x1100) {
    //     EXTRAM[address - 0xF000] = data;
    // }
    // ROM
    if (address >= 0x1000 && address <= 0x1FFF) {
        // LMAO NICE TRY - FAFO
    }

    //BANK
    if (address == 0x3F) {
        ROMBank = (data & 0b00000011);
    }

    //sTr0be BANK
    // The bank number is selected by reading from (or by writing any value to) specific addresses, the addresses and corresponding bank numbers are:
    // Size   Banks  FFF4 FFF5 FFF6 FFF7 FFF8 FFF9 FFFA FFFB
    // 2K,4K  1      -    -    -    -    -    -    -    -
    // 8K     2      -    -    -    -    0    1    -    -
    // 12K    3      -    -    -    -    0    1    2    -
    // 16K    4      -    -    0    1    2    3    -    -
    // 32K    8      0    1    2    3    4    5    6    7
    if (address >= 0x1200 && address <= 0x1FFF) {
        
        // Execute functions 
    }
}