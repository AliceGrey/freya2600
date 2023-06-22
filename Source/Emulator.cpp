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
    FILE *cart = fopen(filename, "rb");

    char title[1024];
    snprintf(title,sizeof(title),"Freya2600 - %s",filename);
    SDL_SetWindowTitle(Window,title);

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
    PC = ReadWord(0xFFFC);
    printf("Entrypoint: %04X\n", PC);
    // BRK ?
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

        // TODO: Determine when a frame has been drawn
        for (int i = 0; i < 1000; ++i) {
            TickCPU();
            TickPPU();
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
    }
}