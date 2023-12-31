#include "Debugger.hpp"
#include "Emulator.hpp"
#include "Font.hpp"
#include "Utility.hpp"

#include <fmt/format.h>

Debugger::Debugger(Emulator * emu)
{
    Emu = emu;

    constexpr int WIDTH = 1024;
    constexpr int HEIGHT = 768; 

    SDL_Rect mainWindow;
    SDL_GetWindowSize(Emu->Window, &mainWindow.w, &mainWindow.h);
    SDL_GetWindowPosition(Emu->Window, &mainWindow.x, &mainWindow.y);

    Window = SDL_CreateWindow(
        "Freya2600 - Debugger",
        mainWindow.x + mainWindow.w,
        100,
        WIDTH,
        HEIGHT,
        SDL_WINDOW_RESIZABLE
    );

    WindowID = SDL_GetWindowID(Window);

    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);

    SDL_RWops * fontRW = SDL_RWFromMem((void *)FONT_BMP, sizeof(FONT_BMP));
    SDL_Surface * fontSurface = SDL_LoadBMP_RW(fontRW, false);

    FontTexture = SDL_CreateTextureFromSurface(Renderer, fontSurface);

    SDL_FreeSurface(fontSurface);
    
    SetTextColor(COLOR_TEXT);

    DefaultMouseCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    HandMouseCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

    // Calling SDL_SetRenderTarget(Renderer, nullptr) will render to the window
    RenderTargets[0] = nullptr;

    for (unsigned i = 0; i < MAX_RENDER_DEPTH; ++i) {
        RenderTargets[i + 1] = SDL_CreateTexture(
            Renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_TARGET,
            WIDTH,
            HEIGHT
        );
    }
}

Debugger::~Debugger()
{
    for (unsigned i = 0; i < MAX_RENDER_DEPTH; ++i) {
        SDL_DestroyTexture(RenderTargets[i + 1]);
        RenderTargets[i + 1] = nullptr;
    }

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
        if (event->motion.windowID == WindowID) {
            Mouse.x = event->motion.x;
            Mouse.y = event->motion.y;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        if (event->button.windowID == WindowID) {
            if (event->button.button == SDL_BUTTON_LEFT) {
                MouseReleased = true;
            }
        }
        break;
    }
}

void Debugger::Render()
{
    // Cursor as in the icon your mouse uses
    SDL_SetCursor(DefaultMouseCursor);

    SetDrawColor(COLOR_BACKGROUND);
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
    DrawIO();

    SetCursor(250, 50);
    DrawTimer();

    MoveCursor(0, FONT_LINE_HEIGHT);
    DrawVideo();

    SetCursor(450, 50);
    DrawRAM();

    MoveCursor(0, FONT_LINE_HEIGHT);

    constexpr int DISASM_PANEL_WIDTH = 500;
    constexpr int DISASM_PANEL_HEIGHT = 450;

    DrawHeading("Disassembly");
    const auto& panel = BeginPanel(DISASM_PANEL_WIDTH, DISASM_PANEL_HEIGHT);

    // TODO: Improve
    SDL_Point panelMouse = Mouse;
    panelMouse.x -= panel.TopLeft.x;
    panelMouse.y -= panel.TopLeft.y;
    
    word entrypoint = Emu->ReadWord(0xFFFC, false);
    word interrupt = Emu->ReadWord(0xFFFE, false);

    uint16_t searchAddress = (Emu->PC & ADDRESS_MASK);

    int disasmHeight = 0;
    bool addressFound = false;
    int addressOffset = 0;
    for (auto& [address, inst] : InstructionMap) {
        if (address == Emu->PC) {
            addressFound = true;
            addressOffset = disasmHeight;
        }
        
        disasmHeight += FONT_LINE_HEIGHT;
        
        if (IsIn(inst.Opcodes[0], {
                0x00, // BRK
                0x4C, // JMP Absolute
                0x6C, // JMP (Absolute)
                0x40, // RTI
                0x60, // RTS
            }))
        {
            disasmHeight += FONT_LINE_HEIGHT;
        }
    }

    if (!addressFound) {
        printf("Unknown address %04X\n", searchAddress);
    }

    static int scroll = 0;

    while (addressOffset < scroll) {
        scroll -= FONT_LINE_HEIGHT;
    }

    while (addressOffset > (scroll + 400)) {
        scroll += FONT_LINE_HEIGHT;
    }

    SetCursor(0, -scroll);

    char buffer[1024];
    unsigned limit = 30;
    for (auto& [address, inst] : InstructionMap) {
        strcpy(buffer, "     ");

        if (address == Breakpoint) {
            buffer[1] = '*';
        }

        if (address == Emu->PC) {
            buffer[3] = '>';
        }

        if (address == entrypoint) {
            strcat(buffer, "START:  ");
        }
        else if (address == interrupt) {
            strcat(buffer, "BREAK:  ");
        }
        else if (inst.JumpDestination) {
            sprintf(buffer + 5, "%04X:   ", address);
        }
        else {
            strcat(buffer, "        ");
        }

        strcat(buffer, inst.ToString());
        strcat(buffer, "\n");
        
        if (IsIn(inst.Opcodes[0], {
                0x00, // BRK
                0x4C, // JMP Absolute
                0x6C, // JMP (Absolute)
                0x40, // RTI
                0x60, // RTS
            }))
        {
            strcat(buffer, "\n");
        }

        if (Cursor.y >= 0) {
            SDL_Rect bounds = {
                .x = Cursor.x,
                .y = Cursor.y,
                .w = 350,
                .h = FONT_LINE_HEIGHT,
            };

            if (SDL_PointInRect(&panelMouse, &bounds)) {
                SetTextColor(COLOR_TEXT_DISABLED);
                SDL_RenderFillRect(Renderer, &bounds);

                if (MouseReleased) {
                    Breakpoint = address;
                }
            }
            else {
                SetTextColor(COLOR_TEXT);
            }
        }

        DrawText(buffer);
    }

    int scrollbarOffset = scroll / DISASM_PANEL_HEIGHT;
    int scrollbarHeight = disasmHeight / DISASM_PANEL_HEIGHT;

    SDL_Rect scrollbar = {
        .x = DISASM_PANEL_WIDTH - 20,
        .y = scrollbarOffset,
        .w = 20,
        .h = scrollbarHeight,
    };

    SetDrawColor(COLOR_BORDER);
    SDL_RenderFillRect(Renderer, &scrollbar);

    EndPanel(panel);

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

    SetDrawColor(COLOR_TEXT);
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
        SetTextColor(COLOR_TEXT_DISABLED);
        SetDrawColor(COLOR_BUTTON_DISABLED);
    }
    else if (hover) {
        SetDrawColor(COLOR_BUTTON_HOVER);
        SDL_SetCursor(HandMouseCursor);
    }
    else {
        SetDrawColor(COLOR_BUTTON);
    }

    SDL_RenderFillRect(Renderer, &bounds);

    SetDrawColor(COLOR_BORDER);
    SDL_RenderDrawRect(Renderer, &bounds);

    MoveCursor(PADDING, PADDING);
    DrawText(label);

    SetCursor(Cursor.x + PADDING + FONT_GLYPH_WIDTH, topLeft.y);
    SetTextColor(COLOR_TEXT);

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

    SDL_Point textSize = MeasureText(label);

    SDL_Rect bounds = {
        .x = topLeft.x,
        .y = topLeft.y,
        .w = textSize.x + checkbox.w + FONT_GLYPH_WIDTH,
        .h = textSize.y,
    };

    bool hover = SDL_PointInRect(&Mouse, &bounds);

    SetDrawColor(COLOR_BORDER);
    SDL_RenderDrawRect(Renderer, &checkbox);

    if (checked) {
        SDL_Rect mark = {
            .x = checkbox.x + 3,
            .y = checkbox.y + 3,
            .w = checkbox.w - 6,
            .h = checkbox.h - 6,
        };

        if (hover) {
            SetDrawColor(COLOR_BUTTON_HOVER);
        }
        else {
            SetDrawColor(COLOR_BUTTON);
        }

        SDL_RenderFillRect(Renderer, &mark);
    }

    MoveCursor(checkbox.w + FONT_GLYPH_WIDTH, 0);

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

Debugger::Panel Debugger::BeginPanel(int width, int height)
{
    constexpr int PADDING = 5;

    ++CurrentRenderTarget;
    if (CurrentRenderTarget > MAX_RENDER_DEPTH) {
        // freak out?
    }

    SDL_SetRenderTarget(Renderer, RenderTargets[CurrentRenderTarget]);

    SetDrawColor(COLOR_BACKGROUND);
    SDL_RenderClear(Renderer);

    SDL_Point topLeft = Cursor;

    // Relative to the render target texture
    SetCursor(0, 0);

    return {
        .TopLeft = topLeft,
        .Size = {
            width,
            height,
        }
    };
}

void Debugger::EndPanel(const Debugger::Panel& panel)
{
    constexpr int PADDING = 5;

    SetCursor(panel.TopLeft.x, panel.TopLeft.y + panel.Size.y + FONT_LINE_HEIGHT);

    unsigned renderSource = CurrentRenderTarget;

    if (CurrentRenderTarget == 0) {
        // freak out?
    }
    --CurrentRenderTarget;

    SDL_SetRenderTarget(Renderer, RenderTargets[CurrentRenderTarget]);

    SDL_Point innerSize = {
        .x = panel.Size.x - (PADDING * 2),
        .y = panel.Size.y - (PADDING * 2),
    };

    SDL_Rect src = {
        .x = 0,
        .y = 0,
        .w = innerSize.x,
        .h = innerSize.y,
    };

    SDL_Rect dst = {
        .x = panel.TopLeft.x + PADDING,
        .y = panel.TopLeft.y + PADDING,
        .w = innerSize.x,
        .h = innerSize.y,
    };

    SDL_RenderCopy(
        Renderer,
        RenderTargets[renderSource],
        &src,
        &dst
    );

    SDL_Rect border = {
        .x = panel.TopLeft.x,
        .y = panel.TopLeft.y,
        .w = panel.Size.x,
        .h = panel.Size.y,
    };

    SetDrawColor(COLOR_BORDER);
    SDL_RenderDrawRect(Renderer, &border);
}

Debugger::TabBar Debugger::BeginTabBar(int width, int height)
{
    SDL_Rect bounds = {
        .x = Cursor.x,
        .y = Cursor.y + (2 * FONT_LINE_HEIGHT),
        .w = width,
        .h = height,
    };

    SetDrawColor(COLOR_BORDER);
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

    SetDrawColor(COLOR_RAM_ADDRESS);
    SetTextColor(COLOR_RAM_ADDRESS);

    SDL_RenderDrawLine(Renderer,
        Cursor.x,
        Cursor.y + FONT_LINE_HEIGHT - 2,
        Cursor.x + (51 * FONT_GLYPH_WIDTH),
        Cursor.y + FONT_LINE_HEIGHT - 2
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

        SetDrawColor(COLOR_BORDER);
        SDL_RenderDrawRect(Renderer, &stack);
    }

    SDL_Point innerGrid = Cursor;

    DrawText("80\n90\nA0\nB0\nC0\nD0\nE0\nF0");

    SetCursor(innerGrid.x, innerGrid.y);
    
    SetTextColor(COLOR_TEXT);
    
    for (int off = 0; off < sizeof(Emu->RAM); off += 16) {
        DrawText(fmt::format(
            "   "
            "{:02X} {:02X} {:02X} {:02X} "
            "{:02X} {:02X} {:02X} {:02X} "
            "{:02X} {:02X} {:02X} {:02X} "
            "{:02X} {:02X} {:02X} {:02X}\n",
            ram[off + 0x0], ram[off + 0x1], ram[off + 0x2], ram[off + 0x3],
            ram[off + 0x4], ram[off + 0x5], ram[off + 0x6], ram[off + 0x7],
            ram[off + 0x8], ram[off + 0x9], ram[off + 0xA], ram[off + 0xB],
            ram[off + 0xC], ram[off + 0xD], ram[off + 0xE], ram[off + 0xF]
        ));
    }
}

void Debugger::DrawTimer()
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

void Debugger::DrawVideo()
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

        SetDrawColor(color);
        SDL_RenderFillRect(Renderer, &preview);

        SetDrawColor(COLOR_BORDER);
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
}

void Debugger::DrawIO()
{

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

    SetCursor(topLeft.x + (12 * FONT_GLYPH_WIDTH), topLeft.y);

    DrawHeading("P1 Joystick");
    DrawCheckbox("Up\n", !Emu->SWCHA.P1Up);
    DrawCheckbox("Down\n", !Emu->SWCHA.P1Down);
    DrawCheckbox("Left\n", !Emu->SWCHA.P1Left);
    DrawCheckbox("Right\n", !Emu->SWCHA.P1Right);

}

void Debugger::Disassemble(word address, bool jumped /*= false*/)
{
    // printf("Disassembling %04X\n", address);

    auto it = InstructionMap.find(address);
    if (it != InstructionMap.end()) {
        if (jumped) {
            it->second.JumpDestination = true;
        }
        
        return;
    }

    // InstructionRecord record(Emu, address);

    // Suck it std::map
    auto [newIt, _] = InstructionMap.emplace(address, InstructionRecord(Emu, address));
    
    InstructionRecord& record = newIt->second;
    
    if (jumped) {
        record.JumpDestination = true;
    }

    // if (!FirstInstruction) {
    //     FirstInstruction = record;
    //     goto next; // fuck it
    // }

    // if (FirstInstruction->Address > address) {
    //     record.Next = FirstInstruction;
    //     FirstInstruction->Prev = record;
    //     FirstInstruction = record;
    //     goto next;
    // }

    // ptr = FirstInstruction;
    // while (ptr->Next) {
    //     if (ptr->Address < address) {
    //         if (ptr->Next->Address > address) {
    //             record.Next = ptr->Next;
    //             record.Prev = ptr;
    //             ptr->Next = record;
    //             goto next;
    //         }
    //     }

    //     ptr = ptr->Next;
    // }

    // ptr->Next = record;
    // record.Prev = ptr;

// next:

    if (record.Opcodes[0] == 0x40 || // RTI
        record.Opcodes[0] == 0x60) { // RTS
        return;
    }
    else if (record.Opcodes[0] == 0x00) { // BRK
        address = Emu->ReadWord(0xFFFE, false);
        Disassemble(address, true);
        return;
    }
    else if (record.Opcodes[0] == 0x4C) { // JMP Absolute
        address = (record.Opcodes[2] << 8) | record.Opcodes[1];
        Disassemble(address, true);
        return;
    }
    else if (record.Opcodes[0] == 0x6C) { // JMP (Absolute)
        address = (record.Opcodes[2] << 8) | record.Opcodes[1];
        address = Emu->ReadWord(address, false);
        Disassemble(address, true);
        return;
    }
    else if (record.Opcodes[0] == 0x20) { // JSR Absolute
        word dest = (record.Opcodes[2] << 8) | record.Opcodes[1];
        Disassemble(dest, true);
    }
    else if (IsIn(record.Opcodes[0], {
            0x10, // BPL
            0x30, // BMI
            0x50, // BVC
            0x70, // BVS
            0x90, // BCC
            0xB0, // BCS
            0xD0, // BNE
            0xF0, // BEQ
        }))
    {
        word dest = address + 2 + (int8_t)record.Opcodes[1];
        Disassemble(dest, true);
    }

    address += record.Definition->ByteCount;
    Disassemble(address);
}

void Debugger::PrintDisassembly()
{
    word entrypoint = Emu->ReadWord(0xFFFC, false);
    word interrupt = Emu->ReadWord(0xFFFE, false);

    // InstructionRecord * ptr = FirstInstruction;
    // while (ptr) {
    
    for (auto& [address, inst] : InstructionMap) {
        if (address == entrypoint) {
            printf("START:  ");
        }
        else if (address == interrupt) {
            printf("BREAK:  ");
        }
        else if (inst.JumpDestination) {
            printf("%04X:   ", address);
        }
        else {
            printf("        ");
        }

        printf("%s\n", inst.ToString());
        
        if (IsIn(inst.Opcodes[0], {
                0x00, // BRK
                0x4C, // JMP Absolute
                0x6C, // JMP (Absolute)
                0x40, // RTI
                0x60, // RTS
            }))
        {
            printf("\n");
        }

        // ptr = ptr->Next;
    }
}

// const char * Debugger::Disassemble(word address)
// {   
//     // TODO: Remove duplicate reads
//     byte d0 = Emu->ReadByte(address + 0, false);
//     byte d1 = Emu->ReadByte(address + 1, false);
//     byte d2 = Emu->ReadByte(address + 2, false);

//     DisassembledInstruction disasm = {
//         .address = address,
//         .opcode = { d0, d1, d2 },
//     };

//     const auto& inst = INSTRUCTIONS[d0];

//     static char disassembly[1024];

//     memset(disassembly, 0, sizeof(disassembly));
//     fmt::format_to_n(disassembly, sizeof(disassembly), fmt::runtime(inst.format), d1, d2, address + (int8_t)d1 + inst.bytes);

//     return disassembly;
// }

// void Debugger::printRegisters()
// {	
//     // TODO: Remove duplicate reads
//     byte d0 = ReadByte(LastInstructionAddress + 0, false);
//     byte d1 = ReadByte(LastInstructionAddress + 1, false);
//     byte d2 = ReadByte(LastInstructionAddress + 2, false);

//     const auto& inst = INSTRUCTIONS[d0];

//     char disassembly[1024];

//     memset(disassembly, 0, sizeof(disassembly));
//     fmt::format_to_n(disassembly, sizeof(disassembly), fmt::runtime(inst.format), d1, d2, LastInstructionAddress + (int8_t)d1 + inst.bytes);

// /*
// 				fprintf(zlog, "\n(%3d %3d %3d) (%3d %3d) (%3d %3d %3d %3d %3d) ",
// 					 (int)frame, line, cycle, line-42, cycle*3-68,
// 					 (P0_Pos-68+5)%160, (P1_Pos-68+5)%160, (M0_Pos-68+4)%160,
// 					 (M1_Pos-68+4)%160, (BL_Pos-68+4)%160);
// */
//     int P0_Position = 0;
//     int P1_Position = 0;
//     int M0_Position = 0;
//     int M1_Position = 0;
//     int BL_Position = 0;
//     int TIACollide = 0;
//     byte B = 1;

// 	int sline = 0;
// 	int cyc = CPUCycleCount;
// 	int clk = CPUCycleCount*3-68;
// 	if (cyc == 76)
// 	{
// 		cyc -= 76;
// 		clk -= 228;
// 		sline++;
// 	}

// 	fprintf(tLog,
//         "(%05x %4d %3d %3d)  (%3d %3d %3d %3d %3d)  <%4x>  ",
// 		(int)INTIM,
//         MemoryLine + 1,
//         (int)(CPUCycleCount - LastWSYNC),
//         (int)TIACycleCount,
// 		(P0_Position - 68 + 5) % 160,
//         (P1_Position - 68 + 5) % 160,
//         (M0_Position - 68 + 4) % 160,
// 		(M1_Position - 68 + 4) % 160,
//         (BL_Position - 68 + 4) % 160,
//         TIACollide
//     );

// 	fprintf(tLog, "%c%c%c%c%c%c%c ",
//         N ? 'N' : 'n',
// 	    V ? 'V' : 'v',
// 	    B ? 'B' : 'b',
// 	    D ? 'D' : 'd',
// 	    I ? 'I' : 'i',
// 	    Z ? 'Z' : 'z',
// 	    C ? 'C' : 'c'
//     );

//     fprintf(tLog, "%02x %02x %02x %02x  ", A, X, Y, SP);
//     fprintf(tLog, "%04x: ", LastInstructionAddress);
//     //fprintf(tLog, "%02x       ", opcode);

//     if (inst.bytes == 1) {
//         fprintf(tLog, "%02x       ", d0);
//     }
//     else if (inst.bytes == 2) {
//         fprintf(tLog, "%02x %02x    ", d0, d1);
//     }
//     else if (inst.bytes == 3) {
//         fprintf(tLog, "%02x %02x %02x ", d0, d1, d2);
//     }

//     fprintf(tLog, "%s\n", Disassemble(LastInstructionAddress));
//     fflush(tLog);
// }