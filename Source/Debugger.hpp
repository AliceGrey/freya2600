#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <Config.hpp>
#include <Disassembly.hpp>

#include <string>
#include <map>

#include <SDL.h>

class Emulator;

class Debugger
{
public:

    inline void SetDrawColor(const SDL_Color& color) {
        SDL_SetRenderDrawColor(
            Renderer,
            color.r,
            color.g,
            color.b,
            0xFF
        );
    }

    inline void SetTextColor(const SDL_Color& color) {
        SDL_SetTextureColorMod(
            FontTexture,
            color.r,
            color.g,
            color.b
        );
    }

    // static constexpr SDL_Color COLOR_TEXT            = { 0x00, 0x00, 0x00 };
    // static constexpr SDL_Color COLOR_BACKGROUND      = { 0xFF, 0xFF, 0xFF };
    // static constexpr SDL_Color COLOR_BORDER          = { 0x00, 0x00, 0x00 };
    // static constexpr SDL_Color COLOR_BUTTON          = { 0xAA, 0xAA, 0xAA };
    // static constexpr SDL_Color COLOR_BUTTON_DISABLED = { 0x44, 0x44, 0x44 };
    // static constexpr SDL_Color COLOR_BUTTON_HOVER    = { 0x88, 0x88, 0x88 };

    static constexpr SDL_Color COLOR_TEXT            = { 0xFF, 0xFF, 0xFF };
    static constexpr SDL_Color COLOR_TEXT_DISABLED   = { 0x88, 0x88, 0x88 };
    static constexpr SDL_Color COLOR_BACKGROUND      = { 0x22, 0x26, 0x21 };
    static constexpr SDL_Color COLOR_BORDER          = { 0xFF, 0xFF, 0xFF };
    static constexpr SDL_Color COLOR_BUTTON          = { 0x72, 0x92, 0xB0 };
    static constexpr SDL_Color COLOR_BUTTON_DISABLED = { 0x69, 0x61, 0x56 };
    static constexpr SDL_Color COLOR_BUTTON_HOVER    = { 0xDB, 0x9E, 0xC8 };
    static constexpr SDL_Color COLOR_RAM_ADDRESS     = { 0xDB, 0x9E, 0xC8 };

    unsigned Breakpoint = UINT_MAX;

    Emulator * Emu;

    SDL_Window * Window;

    unsigned WindowID;

    SDL_Renderer * Renderer;

    SDL_Texture * FontTexture;

    SDL_Cursor * DefaultMouseCursor;

    SDL_Cursor * HandMouseCursor;

    Debugger(Emulator * emu);

    ~Debugger();

    void HandleEvent(SDL_Event * event);

    void Render();

    SDL_Point Cursor;

    SDL_Point Mouse;

    bool MouseReleased = false;

    inline void MoveCursor(int dX, int dY) {
        Cursor.x += dX;
        Cursor.y += dY;
    }

    inline void SetCursor(int x, int y) {
        Cursor.x = x;
        Cursor.y = y;
    }

    SDL_Point MeasureText(const std::string& text);

    void DrawText(const std::string& text);

    void DrawHeading(const std::string& text);

    bool DrawButton(const std::string& label, bool enabled = true);

    bool DrawCheckbox(const std::string& label, bool checked);

    struct Panel
    {
        SDL_Point TopLeft;

        SDL_Point Size;
    };

    Panel BeginPanel(int width, int height);

    void EndPanel(const Panel& panel);

    struct TabBar
    {
        int Index;

        SDL_Point TopLeft;

        SDL_Point Size;
    };

    TabBar BeginTabBar(int width, int height);

    void EndTabBar(const TabBar& tabBar);

    bool DrawTab(TabBar& tabBar, const std::string& label, int selected);

    // void DrawDisassembly();

    void DrawRegisters();

    void DrawRAM();

    void DrawTimer();

    void DrawVideo();

    void DrawIO();

    std::map<word, InstructionRecord> InstructionMap;

    // InstructionRecord * FirstInstruction = nullptr;

    void Disassemble(word address, bool jumped = false);

    void PrintDisassembly();
    
    // const char * Disassemble(word address);
    

    static constexpr unsigned MAX_RENDER_DEPTH = 3;

    SDL_Texture * RenderTargets[MAX_RENDER_DEPTH + 1];

    unsigned CurrentRenderTarget = 0;

}; // class Debugger

#endif // DEBUGGER_HPP