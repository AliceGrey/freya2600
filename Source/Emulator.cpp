#include "Emulator.hpp"

#include <cstdio>
#include <cstring>
#include <cassert>

#include <cstdlib>
#include <ctime>

Emulator::Emulator()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    Window = SDL_CreateWindow(
        "Freya2600",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WindowSize.x,
        WindowSize.y,
        SDL_WINDOW_RESIZABLE
    );

    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);

    SDL_RenderSetVSync(Renderer, 1);

    ScreenTexture = SDL_CreateTexture(Renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    // Test screen buffer
    srand(time(nullptr));
    for (int i = 0; i < SCREEN_BUFFER_SIZE; i += 3) {
        ScreenBuffer[i + 0] = rand() % 256;
        ScreenBuffer[i + 1] = rand() % 256;
        ScreenBuffer[i + 2] = rand() % 256;
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