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

    struct TabBar
    {
        int Index;

        SDL_Point TopLeft;

        SDL_Point Size;
    };

    TabBar BeginTabBar(int width, int height);

    void EndTabBar(TabBar& tabBar);

    bool DrawTab(TabBar& tabBar, const std::string& label, int selected);

    void DrawRegisters();

    void DrawRAM();

    void DrawPIA();

    void DrawTIA();

    std::map<word, InstructionRecord> InstructionMap;

    // InstructionRecord * FirstInstruction = nullptr;

    void Disassemble(word address, bool jumped = false);

    void PrintDisassembly();
    
    // const char * Disassemble(word address);

}; // class Debugger

#endif // DEBUGGER_HPP