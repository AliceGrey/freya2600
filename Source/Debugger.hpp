#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <string>

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

    bool DrawButton(const std::string& label);

    void DrawRegisters();

}; // class Debugger

#endif // DEBUGGER_HPP