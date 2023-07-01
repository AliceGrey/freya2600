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
        100,
        1024,
        768,
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
    // Cursor as in the icon your mouse uses
    SDL_SetCursor(DefaultMouseCursor);

    SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
    SDL_RenderClear(Renderer);
    //Cursor as in where to draw
    SetCursor(10, 10);
    if (DrawButton("Reset")) {
        Emu->Reset();
    }

    if (DrawButton("Play/Pause")) {
        Emu->IsPlaying = !Emu->IsPlaying;
    }

    if (DrawButton("Step", !Emu->IsPlaying)) {
        Emu->DoStep();
    }

    if (DrawButton("Line", !Emu->IsPlaying)) {
        Emu->DoLine();
    }

    if (DrawButton("Frame", !Emu->IsPlaying)) {
        Emu->DoFrame();
    }

    SetCursor(10, 50);
    DrawRegisters();

    MoveCursor(0, FONT_LINE_HEIGHT);

    // static int tab = 0;
    // tab = DrawTabs({ "PIA", "TIA", "RAM" }, tab);
    // switch (tab) {
    // case 0:
        DrawPIA();
    //     break;
    // case 1:
        DrawTIA();
    //     break;
    // case 2:
        SetCursor(200, 50);
        DrawRAM();
    //     break;
    // }

    SDL_RenderPresent(Renderer);

    MouseReleased = false;
}

SDL_Point Debugger::MeasureText(const std::string& text)
{
    if (text.empty()) {
        return { 0, 0 };
    }
    
    int row = 1;
    int col = 0;
    int maxRow = row;
    int maxCol = col;
    
    for (char c : text) {
        if (c == '\n') {
            if (row >= maxRow) {
                ++maxRow;
            }
            ++row;
            col = 0;
        }
        else {
            if (col >= maxCol) {
                ++maxCol;
            }
            ++col;
        }
    }

    if (text.back() == '\n') {
        --maxRow;
    }

    return {
        maxCol * FONT_GLYPH_WIDTH,
        maxRow * FONT_LINE_HEIGHT,
    };
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

void Debugger::DrawHeading(const std::string& text)
{
    SDL_Point topLeft = Cursor;

    DrawText(text);

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0xFF);
    SDL_RenderDrawLine(Renderer,
        topLeft.x,
        topLeft.y + FONT_LINE_HEIGHT,
        Cursor.x,
        topLeft.y + FONT_LINE_HEIGHT
    );

    SetCursor(topLeft.x, topLeft.y + (int)(1.5f * FONT_LINE_HEIGHT));
}

bool Debugger::DrawButton(const std::string& label, bool enabled /*= true*/)
{
    constexpr int PADDING = 5;

    if (label.empty()) {
        return false;
    }

    SDL_Point topLeft = Cursor;

    SDL_Point textSize = MeasureText(label);

    SDL_Rect bounds = {
        .x = topLeft.x,
        .y = topLeft.y,
        .w = textSize.x + (2 * PADDING),
        .h = textSize.y + (2 * PADDING),
    };

    bool hover = SDL_PointInRect(&Mouse, &bounds);

    if (!enabled) {
        SDL_SetRenderDrawColor(Renderer, 0x44, 0x44, 0x44, 0xFF);
    }
    else if (hover) {
        SDL_SetRenderDrawColor(Renderer, 0x88, 0x88, 0x88, 0xFF);
        SDL_SetCursor(HandMouseCursor);
    }
    else {
        SDL_SetRenderDrawColor(Renderer, 0xAA, 0xAA, 0xAA, 0xFF);
    }

    SDL_RenderFillRect(Renderer, &bounds);

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0xFF);
    SDL_RenderDrawRect(Renderer, &bounds);

    MoveCursor(PADDING, PADDING);
    DrawText(label);

    SetCursor(Cursor.x + PADDING + FONT_GLYPH_WIDTH, topLeft.y);

    return (enabled && hover && MouseReleased);
}

bool Debugger::DrawCheckbox(const std::string& label, bool checked)
{
    if (label.empty()) {
        return false;
    }

    SDL_Point topLeft = Cursor;

    SDL_Rect checkbox = {
        .x = topLeft.x + 1,
        .y = topLeft.y + 1,
        .w = FONT_LINE_HEIGHT - 2,
        .h = FONT_LINE_HEIGHT - 2,
    };

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0xFF);
    SDL_RenderDrawRect(Renderer, &checkbox);

    if (checked) {
        SDL_Rect mark = {
            .x = checkbox.x + 3,
            .y = checkbox.y + 3,
            .w = checkbox.w - 6,
            .h = checkbox.h - 6,
        };

        SDL_RenderFillRect(Renderer, &mark);
    }

    MoveCursor(checkbox.w + FONT_GLYPH_WIDTH, 0);

    SDL_Point textSize = MeasureText(label);

    SDL_Rect bounds = {
        .x = topLeft.x,
        .y = topLeft.y,
        .w = textSize.x + checkbox.w + FONT_GLYPH_WIDTH,
        .h = textSize.y,
    };

    bool hover = SDL_PointInRect(&Mouse, &bounds);

    if (hover) {
        SDL_SetCursor(HandMouseCursor);
    }

    DrawText(label);

    if (label.back() == '\n') {
        SetCursor(topLeft.x, Cursor.y + (FONT_LINE_HEIGHT / 2));
    }
    else {
        MoveCursor(FONT_GLYPH_WIDTH, 0);
    }

    return (hover && MouseReleased);
}

Debugger::TabBar Debugger::BeginTabBar(int width, int height)
{
    SDL_Rect bounds = {
        .x = Cursor.x,
        .y = Cursor.y + (2 * FONT_LINE_HEIGHT),
        .w = width,
        .h = height,
    };

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0xFF);
    SDL_RenderDrawRect(Renderer, &bounds);

    return {
        .Index = 0,
        .TopLeft = Cursor,
        .Size = { width, height },
    };
}

bool Debugger::DrawTab(TabBar& tabBar, const std::string& label, int selected)
{
    bool isSelected = (selected == tabBar.Index);
    ++tabBar.Index;

    SDL_Rect bounds = {

    };

    if (isSelected) {

    }
    else {

    }

    return isSelected;
}

void Debugger::DrawRegisters()
{
    DrawHeading("Registers");

    DrawText(fmt::format(
        "PC ${0:04X}\n"
        "SP ${1:02X} #{1:<3d} %{1:08b}\n"
        "A  ${2:02X} #{2:<3d} %{2:08b}\n"
        "X  ${3:02X} #{3:<3d} %{3:08b}\n"
        "Y  ${4:02X} #{4:<3d} %{4:08b}\n",
        Emu->PC,
        Emu->SP,
        Emu->A,
        Emu->X,
        Emu->Y
    ));

    DrawText(fmt::format(
        "SR {}{}-{}{}{}{}{}\n",
        (Emu->N ? 'N' : '-'),
        (Emu->V ? 'V' : '-'),
        (Emu->D ? 'D' : '-'),
        (Emu->B ? 'B' : '-'),
        (Emu->I ? 'I' : '-'),
        (Emu->Z ? 'Z' : '-'),
        (Emu->C ? 'C' : '-')
    ));
}

void Debugger::DrawRAM()
{
    const auto& ram = Emu->RAM;

    DrawHeading("RAM");

    SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0xFF);

    SDL_RenderDrawLine(Renderer,
        Cursor.x,
        Cursor.y + FONT_LINE_HEIGHT - 1,
        Cursor.x + (51 * FONT_GLYPH_WIDTH),
        Cursor.y + FONT_LINE_HEIGHT - 1
    );

    SDL_RenderDrawLine(Renderer,
        Cursor.x + (int)(2.5f * FONT_GLYPH_WIDTH),
        Cursor.y - 1,
        Cursor.x + (int)(2.5f * FONT_GLYPH_WIDTH),
        Cursor.y + (9 * FONT_LINE_HEIGHT) - 1
    );

    DrawText("0x 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");

    if (Emu->SP >= 0x80) {
        SDL_Rect stack = {
            .x = Cursor.x + (3 * FONT_GLYPH_WIDTH) - 2,
            .y = Cursor.y + 1,
            .w = (2 * FONT_GLYPH_WIDTH) + 4,
            .h = FONT_LINE_HEIGHT - 1,
        };

        int row = (Emu->SP / 0x10) - 8;
        int col = (Emu->SP % 0x10);
        stack.x += col * 3 * FONT_GLYPH_WIDTH;
        stack.y += row * FONT_LINE_HEIGHT;

        SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderDrawRect(Renderer, &stack);
    }

    unsigned row = 0x80;
    for (int off = 0; off < sizeof(Emu->RAM); off += 16) {
        DrawText(fmt::format(
            "{:02X} "
            "{:02X} {:02X} {:02X} {:02X} "
            "{:02X} {:02X} {:02X} {:02X} "
            "{:02X} {:02X} {:02X} {:02X} "
            "{:02X} {:02X} {:02X} {:02X}\n",
            row,
            ram[off + 0x0], ram[off + 0x1], ram[off + 0x2], ram[off + 0x3],
            ram[off + 0x4], ram[off + 0x5], ram[off + 0x6], ram[off + 0x7],
            ram[off + 0x8], ram[off + 0x9], ram[off + 0xA], ram[off + 0xB],
            ram[off + 0xC], ram[off + 0xD], ram[off + 0xE], ram[off + 0xF]
        ));
        row += 0x10;
    }
}

void Debugger::DrawPIA()
{
    DrawHeading("Timer");

    DrawText(fmt::format(
        "INTIM    {}\n"
        "TIMINT   {}\n"
        "Counter #{}\n"
        "Divider #{}\n",
        Emu->INTIM,
        Emu->TIMINT._raw,
        Emu->TimerCounter,
        Emu->TimerInterval
    ));
}

void Debugger::DrawTIA()
{
    DrawHeading("Video");

    DrawCheckbox("WSYNC\n", Emu->WSYNC);

    DrawText(fmt::format(
        "Scan Line  {}\n"
        "Scan Cycle {}\n",
        Emu->MemoryLine,
        Emu->MemoryColumn
    ));

    MoveCursor(0, FONT_LINE_HEIGHT);

    SDL_Color colors[] = {
        Emu->GetColor(Emu->COLUP0.Index),
        Emu->GetColor(Emu->COLUP1.Index),
        Emu->GetColor(Emu->COLUPF.Index),
        Emu->GetColor(Emu->COLUBK.Index),
    };

    SDL_Rect preview = {
        .x = Cursor.x + (12 * FONT_GLYPH_WIDTH),
        .y = Cursor.y + 1,
        .w = (FONT_LINE_HEIGHT * 2),
        .h = FONT_LINE_HEIGHT - 2,
    };

    for (int i = 0; i < 4; ++i) {
        const auto& color = colors[i];

        SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.g, 0xFF);
        SDL_RenderFillRect(Renderer, &preview);

        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 0xFF);
        SDL_RenderDrawRect(Renderer, &preview);

        preview.y += FONT_LINE_HEIGHT;
    }

    DrawText(fmt::format(
        "COLUP0 ${:02X}\n"
        "COLUP1 ${:02X}\n"
        "COLUPF ${:02X}\n"
        "COLUBK ${:02X}\n",
        Emu->COLUP0._raw,
        Emu->COLUP1._raw,
        Emu->COLUPF._raw,
        Emu->COLUBK._raw
    ));

    MoveCursor(0, FONT_LINE_HEIGHT);

    DrawHeading("I/O");

    DrawText(fmt::format(
        "SWCHA  %{:08b}\n"
        "SWACNT %{:08b}\n"
        "SWCHB  %{:08b}\n"
        "SWBCNT %{:08b}\n",
        Emu->SWCHA._raw,
        Emu->SWACNT,
        Emu->SWCHB._raw,
        Emu->SWBCNT
    ));

    MoveCursor(0, FONT_LINE_HEIGHT);

    DrawHeading("Switches");

    if (DrawCheckbox("Select\n", !Emu->SWCHB.Select)) {
        Emu->SWCHB.Select = !Emu->SWCHB.Select;
    }
    
    if (DrawCheckbox("Reset\n", !Emu->SWCHB.Reset)) {
        Emu->SWCHB.Reset = !Emu->SWCHB.Reset;
    }

    if (DrawCheckbox("P0 DIff\n", Emu->SWCHB.P0DIFF)) {
        Emu->SWCHB.P0DIFF = !Emu->SWCHB.P0DIFF;
    }

    if (DrawCheckbox("P1 DIff\n", Emu->SWCHB.P1DIFF)) {
        Emu->SWCHB.P1DIFF = !Emu->SWCHB.P1DIFF;
    }

    if (DrawCheckbox("Color\n", Emu->SWCHB.ColorEnabled)) {
        Emu->SWCHB.ColorEnabled = !Emu->SWCHB.ColorEnabled;
    }

    MoveCursor(0, FONT_LINE_HEIGHT);

    SDL_Point topLeft = Cursor;

    DrawHeading("P0 Joystick");
    DrawCheckbox("Up\n", !Emu->SWCHA.P0Up);
    DrawCheckbox("Down\n", !Emu->SWCHA.P0Down);
    DrawCheckbox("Left\n", !Emu->SWCHA.P0Left);
    DrawCheckbox("Right\n", !Emu->SWCHA.P0Right);

    SetCursor(topLeft.x + 100, topLeft.y);

    DrawHeading("P1 Joystick");
    DrawCheckbox("Up\n", !Emu->SWCHA.P1Up);
    DrawCheckbox("Down\n", !Emu->SWCHA.P1Down);
    DrawCheckbox("Left\n", !Emu->SWCHA.P1Left);
    DrawCheckbox("Right\n", !Emu->SWCHA.P1Right);

}