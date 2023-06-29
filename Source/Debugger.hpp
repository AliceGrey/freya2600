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

    Debugger(Emulator * emu);

    ~Debugger();

    void HandleEvent(SDL_Event * event);

    void Render();

    SDL_Point Cursor;

    void MoveCursor(int x, int y);

    void DrawText(const std::string& text);

    void DrawRegisters();

}; // class Debugger

#endif // DEBUGGER_HPP