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

    // Determine the ROM file size
    fseek(file, 0, SEEK_END);
    unsigned long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Calculate the number of ROM banks
    int numBanks = fileSize / ROM_BANK_SIZE;
    if (numBanks > MAX_BANKS) {
        printf("'%s' is not a valid Atari 2600 ROM, the file is too large.\n");
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

    // $FFFC Cartridge Entrypoint
    PC = ReadWord(0xFFFC);
    printf("Entrypoint: %04X\n", PC);
}

void Emulator::Run()
{
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
        while (drawing){

            uint64_t beforeInstCycles = CPUCycleCount;
            
            TickCPU();

            uint64_t deltaInstCycles = CPUCycleCount - beforeInstCycles;

            printf("Instruction took %llu cycles\n", deltaInstCycles);

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

        //printf("Frame took %llu cycles\n", deltaFrameCycles);

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
    printf(sep);

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
        printf(sep);
    }
}