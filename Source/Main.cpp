#include "Emulator.hpp"

#include <thread>
#include <cstdio>

int main(int argc, char * argv[])
{
    Emulator * emu = new Emulator();

    emu->StartDebugger();

    if (argc < 2) {
        fprintf(stderr, "Usage: %s ROM_FILENAME\n", argv[0]);
        return 1;
    }

    emu->LoadCartridge(argv[1]);
    
    emu->Run();

    delete emu;

    return 0;
}