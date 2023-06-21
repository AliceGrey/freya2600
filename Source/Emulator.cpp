#include "Emulator.hpp"

#include <cstdio>
#include <cstring>

Emulator::Emulator()
{

}

Emulator::~Emulator()
{

}

void Emulator::LoadCartridge(const char * filename)
{
    FILE *cart = fopen(filename, "rb");

    fseek(cart, 0, SEEK_END);
    long fz = ftell(cart);
    fseek(cart, 0, SEEK_SET);
    fread(&ROM[0][0x0000], 1, fz, cart);
    fclose(cart);

    if (fz == 0x0800){
        //Because memory banks lmao
        memcpy(&ROM[0][0x0800], &ROM[0][0x0000], 0x0800);
        // so smol
        // much wow
        // very cool
    }

    // $FFFC Cartridge Entrypoint
    PC = ReadWord(0x1FFC);
    printf("Entrypoint: %04X\n", PC);
    // BRK ?
}