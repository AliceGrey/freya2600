#include "Debugger.hpp"
#include "Emulator.hpp"
#include "Font.hpp"

#include <fmt/format.h>

Debugger::Debugger(Emulator * emu)
{
    Emu = emu;

    SDL_Rect mainWindow;
    SDL_GetWindowSize(Emu->Window, &mainWindow.w, &mainWindow.h);
    SDL_GetWindowPosition(Emu->Window, &mainWindow.x, &mainWindow.y);

    Window = SDL_CreateWindow(
        "Freya2600 - Debugger",
        mainWindow.x + mainWindow.w,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_RESIZABLE
    );

    WindowID = SDL_GetWindowID(Window);

    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);

    SDL_RWops * fontRW = SDL_RWFromMem((void *)FONT_BMP, sizeof(FONT_BMP));
    SDL_Surface * fontSurface = SDL_LoadBMP_RW(fontRW, false);

    FontTexture = SDL_CreateTextureFromSurface(Renderer, fontSurface);

    SDL_FreeSurface(fontSurface);
}

Debugger::~Debugger()
{
    SDL_DestroyTexture(FontTexture);
    FontTexture = nullptr;
}

void Debugger::HandleEvent(SDL_Event * event)
{

}

void Debugger::Render()
{
    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    SDL_RenderClear(Renderer);

    MoveCursor(10, 10);
    DrawRegisters();

    SDL_RenderPresent(Renderer);
}

void Debugger::MoveCursor(int x, int y)
{
    Cursor.x = x;
    Cursor.y = y;
}

void Debugger::DrawText(const std::string& text)
{
    int lineStart = Cursor.x;

    for (char c : text) {
        if (c == ' ') {
            Cursor.x += FONT_GLYPH_WIDTH;
        }
        else if (c == '\n') {
            Cursor.x = lineStart;
            Cursor.y += FONT_LINE_HEIGHT;
        }
        else {
            const char * pch = strchr(FONT_GLYPH_LOOKUP, c);
            if (!pch) {
                pch = strchr(FONT_GLYPH_LOOKUP, '?');
            }

            int index = pch - FONT_GLYPH_LOOKUP;

            SDL_Rect src = {
                .x = (index * FONT_GLYPH_WIDTH),
                .y = 0,
                .w = FONT_GLYPH_WIDTH,
                .h = FONT_LINE_HEIGHT,
            };

            SDL_Rect dst = {
                .x = Cursor.x,
                .y = Cursor.y,
                .w = FONT_GLYPH_WIDTH,
                .h = FONT_LINE_HEIGHT,
            };

            SDL_RenderCopy(Renderer, FontTexture, &src, &dst);

            Cursor.x += FONT_GLYPH_WIDTH;
        }
    }
}

void Debugger::DrawRegisters()
{
    DrawText("[Registers]\n");

    DrawText(fmt::format(
        "A={:02X} X={:02X} Y={:02X} SP={:02X} PC={:04X}\n",
        Emu->A, Emu->X, Emu->Y, Emu->SP, Emu->PC
    ));
}