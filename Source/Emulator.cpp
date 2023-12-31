#include "Emulator.hpp"

#include <cstdio>
#include <cstring>
#include <cassert>

Emulator::Emulator()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    Window = SDL_CreateWindow(
        "Freya2600",
        100,
        100,
        WindowSize.x,
        WindowSize.y,
        SDL_WINDOW_RESIZABLE
    );

    WindowID = SDL_GetWindowID(Window);

    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);

    SDL_RenderSetVSync(Renderer, 1);

    ScreenTexture = SDL_CreateTexture(Renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
}

Emulator::~Emulator()
{
    if (Debug) {
        delete Debug;
        Debug = nullptr;
    }

    SDL_DestroyTexture(ScreenTexture);
    ScreenTexture = nullptr;

    SDL_DestroyRenderer(Renderer);
    Renderer = nullptr;

    SDL_DestroyWindow(Window);
    Window = nullptr;

    SDL_Quit();
}

void Emulator::StartDebugger()
{
    Debug = new Debugger(this);
}

void Emulator::Reset()
{
    // $FFFC Cartridge Entrypoint
    PC = ReadWord(0xFFFC, false);
    printf("Entrypoint: %04X\n", PC);
    
    if (Debug) {
        Debug->Disassemble(PC);
        // Debug->PrintDisassembly();
    }

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

    NUSIZ0._raw = 0x00;
    NUSIZ1._raw = 0x00;

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
    SWCHB.Reset = 1;
    SWCHB.Select = 1;
    SWBCNT = 0x00;

    SWCHA._raw = 0xFF;
    SWACNT = 0x00;

    WSYNC = false;

    // Initial version used $FF, all subsequent versions use $00
    memset(RAM, 0x00, sizeof(RAM));

    ROMBank = 0;

    // TODO: EXTRAM ?

    CPUCycleCount = 0;
    TIACycleCount = 0;

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
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Calculate the number of ROM banks
    int numBanks = fileSize / ROM_BANK_SIZE;
    if (numBanks > MAX_BANKS) {
        printf("'%s' is not a valid Atari 2600 ROM, the file is too large.\n", filename);
        exit(1);
    }

    // Read the ROM file into memory
    fileSize = std::min(fileSize, (long)sizeof(ROM));
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

    IsPlaying = true;
    if (Debug) {
        IsPlaying = false;
    }
    
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                    running = false;
                }

                if (event.window.windowID == WindowID) {
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        WindowSize.x = event.window.data1;
                        WindowSize.y = event.window.data2;
                    }
                }
                else if (Debug) {
                    Debug->HandleEvent(&event);
                }
            }
            else {
                if (Debug) {
                    Debug->HandleEvent(&event);
                }
            }
            // TODO: Input
        }

        if (IsPlaying) {
            DoFrame();
        }
        
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

        if (Debug) {
            Debug->Render();
        }
    }
    
}

void Emulator::DoStep()
{
    do {
        uint64_t beforeInstCycles = CPUCycleCount;
        
        TickCPU();

        uint64_t deltaInstCycles = CPUCycleCount - beforeInstCycles;

        for (uint64_t i = 0; i < deltaInstCycles; ++i) {
            TickPIA();
        }

        for (uint64_t i = 0; i < deltaInstCycles * 3; ++i) {
            TickTIA();
        }   
    }
    while (WSYNC);
}

void Emulator::DoLine()
{
    IsDrawing = true;
    while (IsDrawing) {

        uint64_t beforeInstCycles = CPUCycleCount;
        
        TickCPU();

        uint64_t deltaInstCycles = CPUCycleCount - beforeInstCycles;

        for (uint64_t i = 0; i < deltaInstCycles; ++i) {
            TickPIA();
        }

        for (uint64_t i = 0; i < deltaInstCycles * 3; ++i) {
            unsigned lastMemoryLine = MemoryLine;

            TickTIA();

            if (MemoryLine != lastMemoryLine) {
                IsDrawing = false;
            }
        }
    }
}

void Emulator::DoFrame()
{
    IsDrawing = true;
    while (IsDrawing) {

        uint64_t beforeInstCycles = CPUCycleCount;
        
        TickCPU();

        uint64_t deltaInstCycles = CPUCycleCount - beforeInstCycles;

        for (uint64_t i = 0; i < deltaInstCycles; ++i) {
            TickPIA();
        }

        for (uint64_t i = 0; i < deltaInstCycles * 3; ++i) {
            unsigned lastMemoryLine = MemoryLine;

            TickTIA();

            if (MemoryLine == 0 && MemoryLine != lastMemoryLine) {
                IsDrawing = false;
            }
        }
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
    // fprintf(tLog,"Loaded ROM: %s\n\n", filename);
	// 	fprintf(tLog,
	// 			"(Frame Line CPU TIA)  "
	// 			"( P0  P1  M0  M1  BL)  collsn   "
	// 			"flags   A  X  Y SP   Adr  Code\n");
}
