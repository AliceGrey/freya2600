#include "Emulator.hpp"

#include <cstdio>

int main(int argc, char * argv[])
{
    Emulator * emu = new Emulator();

    if (argc < 2) {
        fprintf(stderr, "Usage: %s ROM_FILENAME\n", argv[0]);
        return 1;
    }

    emu->LoadCartridge(argv[1]);

    for (int i = 0; i < 100; ++i) {
        emu->Tick();
    }

    delete emu;

    return 0;
}