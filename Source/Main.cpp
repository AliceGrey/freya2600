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

    int i;
    for (i = 0;; ++i) {
        if (!emu->Tick()) {
            break;
        }
    }

    printf("%d\n", i);

    delete emu;

    return 0;
}