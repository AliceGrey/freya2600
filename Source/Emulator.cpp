#include "Emulator.hpp"

#include <cstdio>
#include <cstring>
#include <cassert>

Emulator::Emulator()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    Window = SDL_CreateWindow(
        "Freya2600",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WindowSize.x,
        WindowSize.y,
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN
    );

    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);

    SDL_RenderSetVSync(Renderer, 1);

    ScreenTexture = SDL_CreateTexture(Renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    // Official Test Pattern ;) 
    for (unsigned y = 0; y < SCREEN_HEIGHT; ++y) {
        for (unsigned x = 0; x < SCREEN_WIDTH; ++x) {
            unsigned offset = ((y * SCREEN_WIDTH) + x) * 3; // RGB
            if (y < 38 || y > 154) {
                ScreenBuffer[offset + 0] = 91; // R
                ScreenBuffer[offset + 1] = 206; // G
                ScreenBuffer[offset + 2] = 250; // B
            }
            else if (y < 76 || y > 116) {
                ScreenBuffer[offset + 0] = 245; // R
                ScreenBuffer[offset + 1] = 169; // G
                ScreenBuffer[offset + 2] = 184; // B
            }
            else {
                ScreenBuffer[offset + 0] = 255; // R
                ScreenBuffer[offset + 1] = 255; // G
                ScreenBuffer[offset + 2] = 255; // B
            }
        }
    }
}

Emulator::~Emulator()
{
    SDL_DestroyTexture(ScreenTexture);
    ScreenTexture = nullptr;

    SDL_DestroyRenderer(Renderer);
    Renderer = nullptr;

    SDL_DestroyWindow(Window);
    Window = nullptr;

    SDL_Quit();
}

void Emulator::Reset()
{
    // $FFFC Cartridge Entrypoint
    PC = ReadWord(0xFFFC);
    printf("Entrypoint: %04X\n", PC);

    SP = 0x00;
    A = 0x00;
    X = 0x00;
    Y = 0x00;
    SR = 0x00;

    INTIM = 0x00;
    TIMINT._raw = 0x00;
    TimerInterval = 1;
    TimerCounter = 0;

    VSYNC._raw = 0x00;
    VBLANK._raw = 0x00;

    NUSIZ0._raw = 0;
    NUSIZ1._raw = 0;

    COLUP0._raw = 0x00;
    COLUP1._raw = 0x00;
    COLUPF._raw = 0x00;
    COLUBK._raw = 0x00;

    GRP0 = 0x00;
    GRP1 = 0x00;

    CTRLPF._raw = 0x00;
    REFP0._raw = 0x00;
    REFP1._raw = 0x00;

    PF[0] = 0x00;
    PF[1] = 0x00;
    PF[2] = 0x00;

    AUDC0._raw = 0x00;
    AUDC1._raw = 0x00;
    AUDF0._raw = 0x00;
    AUDF1._raw = 0x00;
    AUDV0._raw = 0x00;
    AUDV1._raw = 0x00;

    ENAM0._raw = 0x00;
    ENAM1._raw = 0x00;
    ENABL._raw = 0x00;

    HMP0._raw = 0x00;
    HMP1._raw = 0x00;
    HMM0._raw = 0x00;
    HMM1._raw = 0x00;
    HMBL._raw = 0x00;

    VDELP0._raw = 0x00;
    VDELP1._raw = 0x00;
    VDELBL._raw = 0x00;

    RESMP0._raw = 0x00;
    RESMP1._raw = 0x00;

    SWCHB._raw = 0x00;
    SWCHB.ColorEnabled = 1;
    SWCHB.ResetUp = 1;
    SWCHB.SelectUp = 1;

    WSYNC = false;

    // Initial version used $FF, all subsequent versions use $00
    memset(RAM, 0x00, sizeof(RAM));

    ROMBank = 0;

    // TODO: EXTRAM ?

    CPUCycleCount = 0;
    TIACycleCount = 0;
}

void Emulator::LoadCartridge(const char * filename)
{
    //Try to open file
    FILE* file = fopen(filename, "rb");
    if (file == nullptr) {
        printf("Failed to open ROM file: %s\n", filename);
        return;
    }

    //Set Windows Title
    char title[1024];
    snprintf(title,sizeof(title),"Freya2600 - %s",filename);
    SDL_SetWindowTitle(Window,title);

    printTraceLogHeaders(filename);

    // Determine the ROM file size
    fseek(file, 0, SEEK_END);
    unsigned long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Calculate the number of ROM banks
    int numBanks = fileSize / ROM_BANK_SIZE;
    if (numBanks > MAX_BANKS) {
        printf("'%s' is not a valid Atari 2600 ROM, the file is too large.\n", filename);
        exit(1);
    }

    // Read the ROM file into memory
    fileSize = std::min(fileSize, sizeof(ROM));
    fread(&ROM[0][0x0000], 1, fileSize, file);

    // Deal with undersized ROMs
    if (fileSize == ROM_HALF_BANK_SIZE) {
        memcpy(&ROM[0][ROM_HALF_BANK_SIZE], &ROM[0][0x0000], ROM_HALF_BANK_SIZE);
    }

    fclose(file);

    printf("ROM file loaded successfully.\n");
    printf("Number of ROM banks: %d\n", numBanks);

}

void Emulator::Run()
{
    Reset();

    SDL_ShowWindow(Window);
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    WindowSize.x = event.window.data1;
                    WindowSize.y = event.window.data2;
                }
            }
            // TODO: Input
        }

        uint64_t beforeFrameCycles = CPUCycleCount;

        // TODO: Determine when a frame has been drawn
        bool drawing = true;
        // while (drawing) {
        for (int i = 0; i < 710; ++i) {

            uint64_t beforeInstCycles = CPUCycleCount;
            
            TickCPU();

            uint64_t deltaInstCycles = CPUCycleCount - beforeInstCycles;

            printf("Instruction took %lu cycles\n", deltaInstCycles);

            for (uint64_t i = 0; i < deltaInstCycles; ++i) {
                TickPIA();
            }

            while (WSYNC) {
                TickTIA();
            }

            for (uint64_t i = 0; i < deltaInstCycles * 3; ++i) {
                unsigned lastMemoryLine = MemoryLine;

                TickTIA();

                if (MemoryLine == 0 && MemoryLine != lastMemoryLine) {
                    drawing = false;
                    break;
                }
            }
        }

        uint64_t deltaFrameCycles = CPUCycleCount - beforeFrameCycles;

        printf("Frame took %lu cycles\n", deltaFrameCycles);

        exit(0);
        
        int pitch;
        uint8_t * pixels = nullptr;
        SDL_LockTexture(ScreenTexture, nullptr, (void **)&pixels, &pitch);
        {
            assert(pitch != 0);
            memcpy(pixels, ScreenBuffer, SCREEN_BUFFER_SIZE);
        }
        SDL_UnlockTexture(ScreenTexture);


        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderClear(Renderer);

        float zoom = std::min(
            (float)WindowSize.x / (float)DISPLAY_WIDTH,
            (float)WindowSize.y / (float)DISPLAY_HEIGHT
        );

        SDL_Point size = {
            (int)(DISPLAY_WIDTH * zoom),
            (int)(DISPLAY_HEIGHT * zoom)
        };

        SDL_Rect destination = {
            (WindowSize.x - size.x) / 2,
            (WindowSize.y - size.y) / 2,
            size.x,
            size.y
        };
        
        SDL_RenderCopy(Renderer, ScreenTexture, nullptr, &destination);

        SDL_RenderPresent(Renderer);
    }
    
}

void Emulator::printRAMGrid(const uint8_t* RAM) {
    // Print top column names
    printf("    ");
    for (int i = 0x00; i <= 0x0F; i++) {
        printf("%02X ", i);
    }
    printf("\n");

    const char * sep = "   +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+\n";

    // Print separator line
    printf("%s", sep);

    // Print rows
    for (int row = 0x80; row <= 0xF0; row += 0x10) {
        printf("%02x ", row);
        
        // Print values in the row
        for (int col = 0x00; col <= 0x0F; col++) {
            int index = row + col;
            printf("|%02X", (RAM[index-0x80]));
        }
        
        printf("|\n");

        // Print separator line
        printf("%s", sep);
    }
}

void Emulator::printTraceLogHeaders(const char* filename) {
    tLog = fopen("log.txt", "w");
    fprintf(tLog,"Loaded ROM: %s\n\n", filename);
		fprintf(tLog,
				"(Frame Line CPU TIA)  "
				"( P0  P1  M0  M1  BL)  collsn   "
				"flags   A  X  Y SP   Adr  Code\n");

}
