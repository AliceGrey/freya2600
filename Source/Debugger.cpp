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

    DefaultMouseCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    HandMouseCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
}

Debugger::~Debugger()
{
    SDL_FreeCursor(HandMouseCursor);
    SDL_FreeCursor(DefaultMouseCursor);

    SDL_DestroyTexture(FontTexture);
    FontTexture = nullptr;
}

void Debugger::HandleEvent(SDL_Event * event)
{
    switch (event->type)
    {
    case SDL_MOUSEMOTION:
        Mouse.x = event->motion.x;
        Mouse.y = event->motion.y;
        break;
    case SDL_MOUSEBUTTONUP:
        if (event->button.button == SDL_BUTTON_LEFT) {
            MouseReleased = true;
        }
        break;
    }
}

void Debugger::Render()
{
    SDL_SetCursor(DefaultMouseCursor);

    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    SDL_RenderClear(Renderer);

    SetCursor(10, 10);
    DrawRegisters();

    SetCursor(100, 100);
    if (DrawButton("Suck it")) {
        printf("Suck it\n");
    }

    SDL_RenderPresent(Renderer);

    MouseReleased = false;
}

SDL_Point Debugger::MeasureText(const std::string& text)
{
    // TODO: Handle multi-line
    return { (int)(text.size() * FONT_GLYPH_WIDTH), FONT_LINE_HEIGHT };
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

#define PADDING 5

bool Debugger::DrawButton(const std::string& label)
{
    SDL_Point topLeft = Cursor;

    SDL_Point textSize = MeasureText(label);

    SDL_Rect bounds = {
        .x = topLeft.x,
        .y = topLeft.y,
        .w = textSize.x + PADDING + PADDING,
        .h = textSize.y + PADDING + PADDING,
    };

    bool hover = SDL_PointInRect(&Mouse, &bounds);

    if (hover) {
        SDL_SetRenderDrawColor(Renderer, 128, 128, 128, 255);
        SDL_SetCursor(HandMouseCursor);
    }
    else {
        SDL_SetRenderDrawColor(Renderer, 192, 192, 192, 255);
    }

    SDL_RenderFillRect(Renderer, &bounds);

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(Renderer, &bounds);

    MoveCursor(PADDING, PADDING);
    DrawText(label);

    SetCursor(Cursor.x + PADDING + FONT_GLYPH_WIDTH, topLeft.y);

    return (hover && MouseReleased);
}

void Debugger::DrawRegisters()
{
    DrawText("[Registers]\n");

    DrawText(fmt::format(
        "A={:02X} X={:02X} Y={:02X} SP={:02X} PC={:04X}\n",
        Emu->A, Emu->X, Emu->Y, Emu->SP, Emu->PC
    ));
}